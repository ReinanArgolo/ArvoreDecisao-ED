#include "game.h"
#include <stdio.h>
#include <string.h>

int main(void) {
    Node *root = NULL;
    Player player = {0};
    Game game = { .root = NULL, .player = &player };

    printf("Jogo de Territórios (Árvore Binária) - ED\n\n");

    // Captura nome do jogador (opcional)
    printf("Digite seu nome (ou deixe vazio): ");
    if (fgets(player.nome, sizeof(player.nome), stdin)) {
        size_t len = strlen(player.nome);
        if (len && player.nome[len-1] == '\n') player.nome[len-1] = '\0';
    }

    // Tenta carregar árvore anterior; se não houver, cria com 15 nós
    if (!root) {
        Node *loaded = tree_load_from_file("arvore.txt");
        if (loaded) {
            root = loaded;
            printf("Árvore carregada de 'arvore.txt'.\n");
        } else {
            root = tree_build_initial_15();
            if (root) {
                printf("Árvore inicial (15 nós) criada.\n");
            }
        }
    }

    game.root = root;
    int rc = game_run(&game);

    if (game.root) {
        if (tree_save_to_file("arvore.txt", game.root)) {
            printf("Árvore salva em 'arvore.txt'.\n");
        } else {
            printf("Falha ao salvar a árvore.\n");
        }
        tree_free(game.root);
    }

    return rc;
}
