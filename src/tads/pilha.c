#include "pilha.h"
#include <stdlib.h>
#include <stdio.h>

void inicializar_pilha(Pilha *p) {
    p->topo = NULL;
}

void push(Pilha *p, Territorio *t) {
    NoPilha *novo = (NoPilha*) malloc(sizeof(NoPilha));
    novo->terr = t;
    novo->prox = p->topo;
    p->topo = novo;
}

Territorio* pop(Pilha *p) {
    if (pilha_vazia(p)) return NULL;
    
    NoPilha *temp = p->topo;
    Territorio *t = temp->terr;
    p->topo = temp->prox;
    free(temp);
    return t;
}

int pilha_vazia(Pilha *p) {
    return p->topo == NULL;
}

void liberar_pilha(Pilha *p) {
    while (!pilha_vazia(p)) {
        pop(p);
    }
}
