#ifndef GAME_H
#define GAME_H

#include "tree.h"

// Estrutura simples para jogador
typedef struct Player {
	char nome[64];
} Player;

// Estado do jogo contendo a árvore e o jogador
typedef struct Game {
	Node *root;
	Player *player;
} Game;

// Plays the decision tree guessing game in the console.
// Returns 0 on success.
int game_run(Game *game);

#endif // GAME_H
