#ifndef UI_H
#define UI_H

#include "jogo.h"

// Cores ANSI
#define RST  "\x1b[0m"
#define VERM "\x1b[31m"
#define VERD "\x1b[32m"
#define AZUL "\x1b[36m" // Cyan
#define AMA  "\x1b[33m"
#define NEGR "\x1b[1m"

// Desenha a interface completa do jogo
void desenhar_interface(Territorio *raiz, Player *p1, Player *p2, Player *atual, int rodada, char *log_msg);

// Exibe animação de batalha
void animacao_batalha(Player *atk, Player *def, int pc_atk, int pc_def, int venceu);

#endif
