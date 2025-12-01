#include "tree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *dupstr(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s) + 1;
    char *copy = (char*)malloc(len);
    if (copy) memcpy(copy, s, len);
    return copy;
}

static unsigned int next_id_val = 1;

static unsigned int next_id(void) {
    return next_id_val++;
}

Node *tree_create_leaf(const char *text) {
    Node *n = (Node*)calloc(1, sizeof(Node));
    if (!n) return NULL;
    n->id = next_id();
    n->territorioName = dupstr(text);
    n->esq = NULL;
    n->dir = NULL;
    n->estaOcupado = 0;
    n->ehFolha = true;
    return n;
}

Node *tree_create_question(const char *territorioName) {
    Node *n = (Node*)calloc(1, sizeof(Node));
    if (!n) return NULL;
    n->id = next_id();
    n->territorioName = dupstr(territorioName);
    n->esq = NULL;
    n->dir = NULL;
    n->estaOcupado = 0;
    n->ehFolha = false;
    return n;
}

void tree_set_children(Node *parent, Node *esq, Node *dir) {
    if (!parent) return;
    parent->esq = esq;
    parent->dir = dir;
}

bool tree_is_leaf(const Node *n) {
    return n && n->ehFolha;
}

void tree_free(Node *root) {
    if (!root) return;
    tree_free(root->esq);
    tree_free(root->dir);
    free(root->territorioName);
    free(root);
}

// Serialização simples: cada nó como tipo|nome (pré-ordem)
// tipo: I = interno, L = folha. '#' = nulo.
static bool save_preorder(FILE *f, const Node *n) {
    if (!n) {
        fputs("#\n", f);
        return true;
    }
    fputc(n->ehFolha ? 'L' : 'I', f);
    fputc('|', f);
    if (n->territorioName) fputs(n->territorioName, f);
    fputc('\n', f);
    if (!n->ehFolha) {
        if (!save_preorder(f, n->esq)) return false;
        if (!save_preorder(f, n->dir)) return false;
    }
    return true;
}

bool tree_save_to_file(const char *path, const Node *root) {
    FILE *f = fopen(path, "w");
    if (!f) return false;
    bool ok = save_preorder(f, root);
    fclose(f);
    return ok;
}

static Node *load_preorder(FILE *f) {
    char line[1024];
    if (!fgets(line, sizeof(line), f)) return NULL;
    size_t len = strlen(line);
    if (len && line[len-1] == '\n') line[len-1] = '\0';
    if (strcmp(line, "#") == 0) return NULL;
    if (line[0] != 'L' && line[0] != 'I') return NULL;
    char *sep = strchr(line, '|');
    if (!sep) return NULL;
    char type = line[0];
    const char *text = sep + 1;
    if (type == 'L') {
        return tree_create_leaf(text);
    } else {
        Node *q = tree_create_question(text);
        q->esq = load_preorder(f);
        q->dir = load_preorder(f);
        return q;
    }
}

Node *tree_load_from_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return NULL;
    Node *root = load_preorder(f);
    fclose(f);
    return root;
}

Node *tree_build_initial_15(void) {
    // 8 folhas
    Node *l1 = tree_create_leaf("Setor 1");
    Node *l2 = tree_create_leaf("Setor 2");
    Node *l3 = tree_create_leaf("Setor 3");
    Node *l4 = tree_create_leaf("Setor 4");
    Node *l5 = tree_create_leaf("Setor 5");
    Node *l6 = tree_create_leaf("Setor 6");
    Node *l7 = tree_create_leaf("Setor 7");
    Node *l8 = tree_create_leaf("Setor 8");

    // 4 internos do nível 3
    Node *i7 = tree_create_question("Região 7");
    tree_set_children(i7, l1, l2);
    Node *i6 = tree_create_question("Região 6");
    tree_set_children(i6, l3, l4);
    Node *i5 = tree_create_question("Região 5");
    tree_set_children(i5, l5, l6);
    Node *i4 = tree_create_question("Região 4");
    tree_set_children(i4, l7, l8);

    // 2 internos do nível 2
    Node *i3 = tree_create_question("Região 3");
    tree_set_children(i3, i7, i6);
    Node *i2 = tree_create_question("Região 2");
    tree_set_children(i2, i5, i4);

    // raiz (nível 1)
    Node *i1 = tree_create_question("Região 1");
    tree_set_children(i1, i3, i2);

    return i1; // 7 internos + 8 folhas = 15 nós
}

unsigned int tree_count_nodes(const Node *root) {
    if (!root) return 0;
    return 1u + tree_count_nodes(root->esq) + tree_count_nodes(root->dir);
}
