#include "jogador.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

Player* criar_jogador(int id, char *nome, Territorio *pos_inicial) {
    Player *p = (Player*) malloc(sizeof(Player));
    p->id = id;
    strncpy(p->nome, nome, 29);
    p->nome[29] = '\0';
    
    // Valores iniciais padrão
    p->Aatk = 1;
    p->Adef = 1;
    p->Xp = 0;
    p->V = 10;
    
    p->pos = pos_inicial;
    p->spawn = pos_inicial; // ///ADDED: Define spawn inicial ///
    if (p->pos != NULL) {
        p->pos->ocupado = id;
    }

    inicializar_lista(&p->inv);
    inicializar_pilha(&p->historico);
    
    // ///ADDED: Empilhar posição inicial no histórico para permitir retorno se necessário ///
    if (pos_inicial != NULL) {
        push(&p->historico, pos_inicial);
    }

    return p;
}

void destruir_jogador(Player *p) {
    liberar_lista(&p->inv);
    liberar_pilha(&p->historico);
    free(p);
}

void imprimir_status(Player *p) {
    printf("\n=== Status de %s (P%d) ===\n", p->nome, p->id);
    printf("Posicao: %s (Nivel %d)\n", p->pos->nome, p->pos->nivel);
    printf("Vitalidade: %d | XP: %d\n", p->V, p->Xp);
    printf("Atk: %d | Def: %d\n", p->Aatk, p->Adef);
    printf("Inventario: ");
    listar_itens(&p->inv);
    printf("\n==========================\n");
}
