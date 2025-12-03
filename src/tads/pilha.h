#ifndef PILHA_H
#define PILHA_H

#include "../arvore.h"

typedef struct NoPilha {
    Territorio *terr;
    struct NoPilha *prox;
} NoPilha;

typedef struct {
    NoPilha *topo;
} Pilha;

void inicializar_pilha(Pilha *p);
void push(Pilha *p, Territorio *t);
Territorio* pop(Pilha *p);
int pilha_vazia(Pilha *p);
void liberar_pilha(Pilha *p);

#endif
