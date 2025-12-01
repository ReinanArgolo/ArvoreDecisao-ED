#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static void print_preorder(const Node *n, int depth) {
    if (!n) return;
    for (int i = 0; i < depth; ++i) putchar(' ');
    printf("- [%u] %s%s\n", n->id, n->territorioName ? n->territorioName : "(sem nome)", n->ehFolha ? " (folha)" : "");
    if (!n->ehFolha) {
        print_preorder(n->esq, depth + 2);
        print_preorder(n->dir, depth + 2);
    }
}

int game_run(Game *game) {
    if (!game) return 1;
    if (!game->root) {
        game->root = tree_build_initial_15();
    }

    unsigned int total = tree_count_nodes(game->root);
    if (game->player && game->player->nome[0] != '\0') {
        printf("Bem-vindo, %s!\n", game->player->nome);
    }
    printf("Árvore de Territórios inicializada com %u nós.\n", total);
    printf("Estrutura (pré-ordem):\n");
    print_preorder(game->root, 0);

    // Nesta etapa inicial, apenas exibimos a árvore
    return 0;
}
