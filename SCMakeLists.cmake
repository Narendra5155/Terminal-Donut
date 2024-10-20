cmake_minimum_required(VERSION 3.0.0...3.29.3)
project(Donut VERSION 0.1.0 LANGUAGES C)

include(CTest)
enable_testing()


add_executable(main main.c)
if (UNIX)
target_link_libraries(main m)
endif (UNIX)
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)