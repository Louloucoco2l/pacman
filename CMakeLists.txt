cmake_minimum_required(VERSION 3.25)
project(pacman C)

set(CMAKE_C_STANDARD 11)

add_executable(pacman main.c generation_fusion_aleatoire.c generation_fusion_aleatoire.h global.h generation_division_recursive.c generation_division_recursive.h global.c global.c)
target_link_libraries(pacman -lmingw32 -lalleg44)
