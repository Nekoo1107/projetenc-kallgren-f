cmake_minimum_required(VERSION 3.30)
project(projet_c_processing_image)
set(CMAKE_C_STANDARD 17)
add_executable(main main.c bmp8.c bmp8.h bmp24.c bmp24.h)