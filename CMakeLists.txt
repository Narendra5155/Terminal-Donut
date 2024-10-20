cmake_minimum_required(VERSION 3.10...3.31)
project(Donut VERSION 0.1.0 LANGUAGES C)

add_executable(Donut main.c)

include(CTest)
enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
