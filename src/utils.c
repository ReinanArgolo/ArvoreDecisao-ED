#include "utils.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

void inicializar_rng() {
    srand(time(NULL));
}

int rng_int(int min, int max) {
    return min + rand() % (max - min + 1);
}

void limpar_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void limpar_tela() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}
