cmake_minimum_required(VERSION 3.20)

project(Prog2Trabalho C)

set(CMAKE_C_STANDARD 99)

set(PKG_CONFIG_EXECUTABLE "C:/msys64/ucrt64/bin/pkg-config.exe")

find_package(PkgConfig REQUIRED)
pkg_check_modules(ALLEGRO REQUIRED allegro-5 allegro_main-5 allegro_font-5 allegro_image-5 allegro_primitives-5)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_executable(Prog2Trabalho src/main.c
        src/player.c
        src/player.h
        src/mapa.c
        src/mapa.h
        src/main.c)

target_include_directories(Prog2Trabalho PRIVATE ${ALLEGRO_INCLUDE_DIRS})
target_link_libraries(Prog2Trabalho PRIVATE ${ALLEGRO_LIBRARIES})
target_compile_options(Prog2Trabalho PRIVATE ${ALLEGRO_CFLAGS_OTHER})