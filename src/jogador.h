#ifndef JOGADOR_H
#define JOGADOR_H

#include "arvore.h"
#include "tads/lista.h"
#include "tads/pilha.h"

typedef struct Player {
    int id; // 1 ou 2
    char nome[30];
    int Aatk;
    int Adef;
    int Xp;
    int V; // vitalidade
    Territorio *pos; // ponteiro para Territorio atual
    Territorio *spawn; // ///ADDED: Ponto de respawn (início) ///
    Lista inv; // TAD Lista (Inventario)
    Pilha historico; // TAD Pilha (Historico de posições)
} Player;

Player* criar_jogador(int id, char *nome, Territorio *pos_inicial);
void destruir_jogador(Player *p);
void imprimir_status(Player *p, int producao_farms);

#endif
