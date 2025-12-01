#include "game.h"
#include <stdio.h>

int main(void) {
    Node *root = NULL;

    printf("Jogo da Árvore de Decisão (ED)\n");
    printf("Responda com 's' (sim) ou 'n' (não).\n\n");

    // Try to load previous tree
    if (!root) {
        Node *loaded = tree_load_from_file("arvore.txt");
        if (loaded) {
            root = loaded;
            printf("Árvore carregada de 'arvore.txt'.\n");
        }
    }

    int rc = game_run(&root);

    if (root) {
        if (tree_save_to_file("arvore.txt", root)) {
            printf("Árvore salva em 'arvore.txt'.\n");
        } else {
            printf("Falha ao salvar a árvore.\n");
        }
        tree_free(root);
    }

    return rc;
}
