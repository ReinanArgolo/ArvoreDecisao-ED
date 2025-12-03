#include "lista.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void inicializar_lista(Lista *l) {
    l->inicio = NULL;
    l->tamanho = 0;
}

void adicionar_item(Lista *l, Item item) {
    NoLista *novo = (NoLista*) malloc(sizeof(NoLista));
    novo->item = item;
    novo->prox = l->inicio;
    l->inicio = novo;
    l->tamanho++;
}

int consumir_item(Lista *l, TipoItem tipo) {
    NoLista *atual = l->inicio;
    NoLista *ant = NULL;

    while (atual != NULL) {
        if (atual->item.tipo == tipo) {
            int bonus = atual->item.valor_bonus;
            
            if (ant == NULL) {
                l->inicio = atual->prox;
            } else {
                ant->prox = atual->prox;
            }
            
            free(atual);
            l->tamanho--;
            return bonus;
        }
        ant = atual;
        atual = atual->prox;
    }
    return 0;
}

void liberar_lista(Lista *l) {
    NoLista *atual = l->inicio;
    while (atual != NULL) {
        NoLista *temp = atual;
        atual = atual->prox;
        free(temp);
    }
    l->inicio = NULL;
    l->tamanho = 0;
}

void listar_itens(Lista *l) {
    NoLista *atual = l->inicio;
    if (atual == NULL) {
        printf(" (Vazio)");
        return;
    }
    while (atual != NULL) {
        char *tipoStr;
        switch(atual->item.tipo) {
            case ARMA: tipoStr = "ARMA"; break;
            case ESCUDO: tipoStr = "ESCUDO"; break;
            case ENERGIA: tipoStr = "ENERGIA"; break;
            default: tipoStr = "ITEM";
        }
        printf(" [%s: %s (+%d)]", tipoStr, atual->item.nome, atual->item.valor_bonus);
        atual = atual->prox;
    }
}
