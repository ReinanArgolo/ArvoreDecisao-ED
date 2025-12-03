#include "fila.h"
#include <stdlib.h>
#include <stdio.h>

void inicializar_fila(Fila *f) {
    f->inicio = NULL;
    f->fim = NULL;
}

void enqueue(Fila *f, struct Player *p) {
    NoFila *novo = (NoFila*) malloc(sizeof(NoFila));
    novo->player = p;
    novo->prox = NULL;
    
    if (f->fim == NULL) {
        f->inicio = novo;
    } else {
        f->fim->prox = novo;
    }
    f->fim = novo;
}

struct Player* dequeue(Fila *f) {
    if (fila_vazia(f)) return NULL;
    
    NoFila *temp = f->inicio;
    struct Player *p = temp->player;
    
    f->inicio = temp->prox;
    if (f->inicio == NULL) {
        f->fim = NULL;
    }
    
    free(temp);
    return p;
}

int fila_vazia(Fila *f) {
    return f->inicio == NULL;
}

void liberar_fila(Fila *f) {
    while (!fila_vazia(f)) {
        dequeue(f);
    }
}
