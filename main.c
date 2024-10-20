#include<stdio.h>
#include<windows.h>
#include<wchar.h>
#include<math.h>
#include<stdlib.h>

typedef struct Buffer{
    wchar_t *buff;
    float *zbuff;
    int h;
    int w;
}Buffer;

typedef struct TrigRaito{
    float sa,sb,sc,ca,cb,cc;
}TrigRaito;

wchar_t background=L'.';
const float Twopi=2*3.14159265359;
const float radtoDEG=180/3.14159265359;
const float pi=3.14159265359;
float A=0.0;
float B=0.0;
float C=0.0;
float lightvector[3]={1,1,-1};
float distance=200;
float K=100;
float horizontal_offset=0;
float vertical_offset=0;
int L_len=5;
wchar_t map[50]=L" ░▒▓█";

//X axis Rotation Calculation function(Matrix multipication)
float calcX(float x,float y,float z,TrigRaito *pre){
    return pre->cc * pre->cb * x + pre->cc * pre->sb * pre->sa * y - pre->cc * pre->sb * pre->ca * z + 
           pre->sc * pre->ca * y + pre->sc * pre->sa * z;
}


//Y axis Rotation Calculation function(Matrix multipication)

float calcY(float x,float y,float z,TrigRaito *pre){
    return pre->cc * pre->ca * y + pre->cc * pre->sa * z - pre->sc * pre->cb * x - 
           pre->sc * pre->sb * pre->sa * y + 
           pre->sc * pre->sb * pre->ca * z ;
}

//Z axis Rotation Calculation function(Matrix multipication)

float calcZ(float x,float y,float z,TrigRaito *pre){
    return pre->sb * x - pre->cb * pre->sa * y + pre->cb * pre->ca * z;
}

void CalculateTrig(TrigRaito *pre){
    pre->sa=sin(A);
    pre->sb=sin(B);
    pre->sc=sin(C);
    pre->ca=cos(A);
    pre->cb=cos(B);
    pre->cc=cos(C);

}
void Createbuffer(Buffer *buf){
    /* free(buf->buff);
    free(buf->zbuff); */
    int size=buf->h*buf->w;
    buf->buff=(wchar_t*)malloc(size*sizeof(wchar_t));
    buf->zbuff=(float*)malloc(size*sizeof(float));
}

void Resetbuffer(Buffer *buf){
    int size=buf->h*buf->w;
    memset(buf->zbuff,0,size*sizeof(float));
    wmemset(buf->buff,background,size);
}
static inline void printtoScreen(const wchar_t *str, wchar_t *screen)
{
    int n = wcslen(str);
    for (int i = 0; i < n ;i++){
        screen[i] = str[i];
    }
}

int main(){
    HANDLE stdHand=GetStdHandle(STD_INPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO info;
    DWORD oldMode,newMode;
    newMode=ENABLE_EXTENDED_FLAGS|ENABLE_MOUSE_INPUT|ENABLE_WINDOW_INPUT|ENABLE_PROCESSED_INPUT;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),&info);

    wchar_t *Screen;
    int swidth,sheight;
    /* swidth=163;
    sheight=44;
     */swidth=info.dwSize.X;
    sheight=info.dwSize.Y;
    Screen=(wchar_t*)malloc(swidth*sheight*sizeof(wchar_t));
    wmemset(Screen,background,swidth*sheight);

    Buffer buff1;
    buff1.w=info.dwSize.X;
    buff1.h=info.dwSize.Y;
    
    /* buff1.w=163;
    buff1.h=44;
     */Createbuffer(&buff1);
    Resetbuffer(&buff1);

    SetConsoleMode(stdHand,newMode);
    HANDLE console=CreateConsoleScreenBuffer(GENERIC_READ|GENERIC_WRITE,FILE_SHARE_WRITE|FILE_SHARE_READ,NULL,CONSOLE_TEXTMODE_BUFFER,NULL);
    DWORD conMode=ENABLE_EXTENDED_FLAGS|ENABLE_MOUSE_INPUT|ENABLE_WINDOW_INPUT|ENABLE_PROCESSED_INPUT;
    SetConsoleMode(console,conMode);
    SetConsoleActiveScreenBuffer(console);
    //WINBOOL han=SetStdHandle(STD_INPUT_HANDLE,console);
    COORD origin={0,0};
    DWORD wordswritten,num;
    INPUT_RECORD inRecord[128];


    float R1,R2;
    R1=20;
    R2=8;
    const float increment =0.03;
    TrigRaito angle;
    const float lightmagnitude=sqrt(lightvector[0]*lightvector[0]+
                                    lightvector[1]*lightvector[1]+
                                    lightvector[2]*lightvector[2]);
    const float ldirection[3]={lightvector[0]/lightmagnitude,
                                lightvector[1]/lightmagnitude,
                                lightvector[2]/lightmagnitude};

    LARGE_INTEGER start,end,frequency;
    LONGLONG elapsedticks;
    double deltatime=0;
    int loopOver=0;
    int pauseFlag=0;
    double collectedtime;

    while(!loopOver){

        QueryPerformanceCounter(&start);
        QueryPerformanceFrequency(&frequency);


        GetNumberOfConsoleInputEvents(stdHand,&num);
        if(num>0){
            ReadConsoleInput(stdHand,inRecord,128,&num);
            for(unsigned int i=0;i<num;i++){
                switch(inRecord->EventType){
                    case WINDOW_BUFFER_SIZE_EVENT:
                    if(!(inRecord[i].Event.WindowBufferSizeEvent.dwSize.X==0 ||inRecord[i].Event.WindowBufferSizeEvent.dwSize.Y==0)){
                        swidth=inRecord[i].Event.WindowBufferSizeEvent.dwSize.X;
                        sheight=inRecord[i].Event.WindowBufferSizeEvent.dwSize.Y;
                        buff1.w=inRecord[i].Event.WindowBufferSizeEvent.dwSize.X;
                        buff1.h=inRecord[i].Event.WindowBufferSizeEvent.dwSize.Y;
                        free(buff1.buff);
                        free(buff1.zbuff);
                        free(Screen);
                        Createbuffer(&buff1);
                        Screen=(wchar_t*)malloc(swidth*sheight*sizeof(wchar_t));
                    }
                    break;
                    case MOUSE_EVENT:
                        if(inRecord[i].Event.MouseEvent.dwButtonState==FROM_LEFT_1ST_BUTTON_PRESSED){
                            horizontal_offset=inRecord[i].Event.MouseEvent.dwMousePosition.X-swidth/2;
                            vertical_offset=inRecord[i].Event.MouseEvent.dwMousePosition.Y-sheight/2;
                        }
                    break;
                    case KEY_EVENT:
                        switch(inRecord[i].Event.KeyEvent.wVirtualKeyCode){
                            case VK_SPACE:
                                loopOver=1;
                            break;
                            case 'P':
                            if(!inRecord[i].Event.KeyEvent.bKeyDown)
                                pauseFlag=(pauseFlag==1)?0:1;
                            break;
                            case 'A':
                                if(pauseFlag)
                                    A+=0.5*collectedtime;
                            break;
                            case 'D':
                                if(pauseFlag)
                                    A-=0.5*collectedtime;
                            break;
                            case 'W':
                                if(pauseFlag)
                                    B+=0.5*collectedtime;
                            break;
                            case 'S':
                                if(pauseFlag)
                                    B-=0.5*collectedtime;
                            break;
                            case 'Q':
                                if(pauseFlag)
                                    C+=0.5*collectedtime;
                            break;
                            case 'E':
                                if(pauseFlag)
                                    C-=0.5*collectedtime;
                            break;
                            case'K':
                                if(pauseFlag)
                                    K+=20*collectedtime;
                            break;
                            case'J':
                                if(pauseFlag)
                                    K-=20*collectedtime;
                            break;
                            case'C':
                                if(pauseFlag)
                                    distance-=20*collectedtime;
                            break;
                            case'V':
                                if(pauseFlag)
                                    distance+=20*collectedtime;
                            break;
                            case'M':
                                if(pauseFlag)
                                    R1+=5*collectedtime;
                            break;
                            case'N':
                                if(pauseFlag)
                                    R1-=5*collectedtime;
                            break;
                            case'Y':
                                if(pauseFlag)
                                    R2-=5*collectedtime;
                            break;
                            case'U':
                                if(pauseFlag)
                                    R2+=5*collectedtime;
                            break;
                            case'R':
                                if(!inRecord[i].Event.KeyEvent.bKeyDown){
                                    horizontal_offset=0;
                                    vertical_offset=0;
                                    A=0;
                                    B=0;
                                    C=0;
                                    R1=20;
                                    R2=8;
                                    distance=200;
                                    K=100;
                                }
                            break;
                        }  
                    break;
                    
                }
            }
            collectedtime=0;
        }
        else
            collectedtime+=(collectedtime<0.1)?deltatime:0;

        Resetbuffer(&buff1);
        wmemset(Screen,background,swidth*sheight);

        CalculateTrig(&angle);

        for(float T=0;T<Twopi;T+=increment){
            for(float P=0;P<Twopi;P+=increment){
                float x=cos(P)*(R1+R2*cos(T));
                float y=R2*sin(T);
                float z=sin(P)*(R2*cos(T)+R1);
                float xl=calcX(x,y,z,&angle);
                float yl=calcY(x,y,z,&angle);
                float zl=calcZ(x,y,z,&angle)+distance;
                float nx=cos(T)*cos(P);
                float ny=sin(T);
                float nz=cos(T)*sin(P);
                float nxl=calcX(nx,ny,nz,&angle);
                float nyl=calcY(nx,ny,nz,&angle);
                float nzl=calcZ(nx,ny,nz,&angle);
                float L=nxl*ldirection[0]+nyl*ldirection[1]+nzl*ldirection[2];
                float iz=1/zl;
                int xp=buff1.w/2+horizontal_offset+(xl*K*iz*2);
                int yp=buff1.h/2+vertical_offset-(yl*K*iz);
                int idx=yp*buff1.w+xp;
                int aidx;
                if(L>0){
                    aidx=L_len*L;
                }
                else
                    aidx=0;
                if(idx>0 && idx<(buff1.h*buff1.w) && buff1.zbuff[idx]<iz && xp>=0 && xp<buff1.w){
                    buff1.buff[idx]=map[aidx];
                    buff1.zbuff[idx]=iz;
                }

            }
        }
            for(int j=0;j<swidth&&j<buff1.w;j++){
                for(int k=0;k<sheight && k<buff1.h ;k++){
                    Screen[k*swidth+j]=buff1.buff[k*swidth+j];
                }
            }

            if(!pauseFlag){
                A+=1*deltatime;
                B+=2*deltatime;
                C+=0.4*deltatime;
                wchar_t out[100];
                swprintf(out, 100, L"[FPS : %5d]", (int)(1 / deltatime));
                printtoScreen(out, Screen + 0 * swidth);
                swprintf(out,100,L"[Press R to Refresh]");
                printtoScreen(out, Screen + 1 * swidth);
                swprintf(out,100,L"[Press P to Pause]");
                printtoScreen(out, Screen + 2 * swidth);
            }
            else{
                wchar_t out[100];
                swprintf(out,100,L"[X Rotation : %5.2f°,S-W]",A*radtoDEG);
                printtoScreen(out,Screen+0*swidth);
                swprintf(out,100,L"[Y Rotation : %5.2f°,A-D]",B*radtoDEG);
                printtoScreen(out,Screen+1*swidth);
                swprintf(out,100,L"[Z Rotation : %5.2f°,Q-E]",C*radtoDEG);
                printtoScreen(out,Screen+2*swidth);
                swprintf(out,100,L"[K value    : %5.2f ,J-K]",K);
                printtoScreen(out,Screen+3*swidth);
                swprintf(out,100,L"[Camera     : %5.2f ,C-V]",distance);
                printtoScreen(out,Screen+4*swidth);
                swprintf(out,100,L"[FPS        : %5d     ]",(int)(1/deltatime));
                printtoScreen(out,Screen+5*swidth);
                swprintf(out,100,L"[R1:%3.2f  R2:%3.2f,N-M,Y-U]",R1,R2);
                printtoScreen(out,Screen+6*swidth);
                swprintf(out,100,L"[Press P to Resume      ]");
                printtoScreen(out,Screen+7*swidth);
            }
            WriteConsoleOutputCharacterW(console,Screen,swidth*sheight,origin,&wordswritten);

            QueryPerformanceCounter(&end);
            elapsedticks=end.QuadPart-start.QuadPart;
            deltatime=(double)elapsedticks/frequency.QuadPart;



    }
    SetConsoleActiveScreenBuffer(GetStdHandle(STD_OUTPUT_HANDLE)); 
    //FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
    SetConsoleMode(stdHand,oldMode);
    //Free the console handle 
    CloseHandle(console);
    //system("cls");
    //Print the last known resolution and FPS
    printf("Screen Width : %d\nScreen Height: %d\nFPS :%.2f\n",swidth,sheight,1/deltatime);
        

}
