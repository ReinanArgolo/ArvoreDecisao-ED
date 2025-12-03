#ifndef JOGO_H
#define JOGO_H

#include "arvore.h"
#include "jogador.h"
#include "tads/fila.h"

// Constantes do relatório
#define DELTA_DEFESA 2
#define OMEGA_XP_NUM 1
#define OMEGA_XP_DEN 2
#define K_AMORTECIMENTO 2

// Tenta mover o jogador para o pai. Retorna 1 se sucesso, 0 se falha (restrição ou batalha perdida).
int mover_para_pai(Player *p, Territorio *raiz, Player *adversario, char *log_buffer);

// ///ADDED: Tenta mover lateralmente ///
int mover_lateralmente(Player *p, Territorio *raiz, Player *adversario, int direcao, char *log_buffer);

// Inicia batalha entre atacante e defensor.
void iniciar_batalha(Player *atk, Player *def);

// ///ADDED: Gerencia Farms ///
void gerenciar_farm(Player *p, char *log_buffer);

// ///ADDED: Compra Atributos ///
void comprar_atributos(Player *p, int *compras_feitas, char *log_buffer);

// ///ADDED: Coleta XP das Farms ///
void coletar_xp_farms(Territorio *raiz, Player *p);

// Aplica efeito do vestígio do território.
void aplicar_vestigio(Player *p, Territorio *t, char *log_buffer);

// Loop principal do jogo. Se modo_demo = 1, joga sozinho.
void game_loop(Territorio *raiz, Player *p1, Player *p2, int modo_demo);

// Verifica se o jogador morreu
int verificar_morte(Player *p);

// Verifica se o jogador venceu (está na raiz)
int verificar_vitoria(Player *p, Territorio *raiz);

#endif
