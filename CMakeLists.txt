cmake_minimum_required(VERSION 3.20)

project(Prog2Trabalho C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(ALLEGRO REQUIRED
        allegro-5
        allegro_main-5
        allegro_font-5
        allegro_image-5
        allegro_primitives-5
)
pkg_check_modules(CURL REQUIRED libcurl)
pkg_check_modules(CJSON REQUIRED libcjson)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_executable(Prog2Trabalho
        src/main.c
        src/jogo.c
        src/player.c
        src/mapa.c
        src/interior.c
        src/colisao.c
        src/inimigo.c
        src/pokeapi.c
        src/nivel.c
        src/combate.c
        src/render.c
)
if(APPLE)
    set(ENV{PKG_CONFIG_PATH} "/opt/homebrew/lib/pkgconfig")
    target_link_directories(Prog2Trabalho PRIVATE /opt/homebrew/lib)
endif()

target_include_directories(Prog2Trabalho PRIVATE ${ALLEGRO_INCLUDE_DIRS} ${CURL_INCLUDE_DIRS} ${CJSON_INCLUDE_DIRS})
target_link_libraries(Prog2Trabalho PRIVATE ${ALLEGRO_LIBRARIES} ${CURL_LIBRARIES} ${CJSON_LIBRARIES})
target_compile_options(Prog2Trabalho PRIVATE ${ALLEGRO_CFLAGS_OTHER})
