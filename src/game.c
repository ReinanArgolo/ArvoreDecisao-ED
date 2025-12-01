#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int ask_yes_no(const char *prompt) {
    char buf[64];
    while (1) {
        printf("%s (s/n): ", prompt);
        if (!fgets(buf, sizeof(buf), stdin)) return 0;
        if (buf[0] == 's' || buf[0] == 'S' || buf[0] == 'y' || buf[0] == 'Y') return 1;
        if (buf[0] == 'n' || buf[0] == 'N') return 0;
        printf("Resposta inválida. Digite 's' ou 'n'.\n");
    }
}

static char *read_line(const char *prompt) {
    printf("%s", prompt);
    char tmp[1024];
    if (!fgets(tmp, sizeof(tmp), stdin)) return NULL;
    size_t len = strlen(tmp);
    if (len && tmp[len-1] == '\n') tmp[len-1] = '\0';
    char *out = (char*)malloc(len + 1);
    if (out) memcpy(out, tmp, len + 1);
    return out;
}

int game_run(Node **root) {
    if (!root) return 1;
    if (!*root) {
        // Initialize with a single guess
        *root = tree_create_leaf("gato");
    }

    Node *current = *root;
    while (!tree_is_leaf(current)) {
        int yes = ask_yes_no(current->question);
        current = yes ? current->yes : current->no;
        if (!current) {
            // If branch missing, stop
            printf("Árvore incompleta. Encerrando.\n");
            return 1;
        }
    }

    // Guessing
    char prompt[1024];
    snprintf(prompt, sizeof(prompt), "Você está pensando em %s?", current->question);
    int correct = ask_yes_no(prompt);
    if (correct) {
        printf("Acertei! :)\n");
        return 0;
    }

    // Learn new item and question
    char *real = read_line("Qual era o objeto/animal correto? ");
    if (!real) return 1;
    char *q = read_line("Digite uma pergunta que diferencia (resposta 'sim' para o novo): ");
    if (!q) { free(real); return 1; }

    // Build new nodes
    Node *oldLeaf = tree_create_leaf(current->question);
    Node *newLeaf = tree_create_leaf(real);
    Node *question = tree_create_question(q);

    // If answer 'sim' leads to new item
    tree_set_children(question, newLeaf, oldLeaf);

    // Replace current leaf's content with question node
    // Efficient approach: overwrite current node
    free(current->question);
    current->question = question->question; // take ownership
    current->isLeaf = false;
    current->yes = question->yes;
    current->no = question->no;
    free(question); // question struct freed; strings/children kept

    free(real);
    free(q);

    printf("Obrigado! Aprendi algo novo. :)\n");
    return 0;
}
