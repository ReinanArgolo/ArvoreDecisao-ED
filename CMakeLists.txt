cmake_minimum_required(VERSION 3.15)
project(JogoArvoreDecisao C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(jogo
    src/main.c
    src/tree.c
    src/game.c
)

target_include_directories(jogo PRIVATE include)
