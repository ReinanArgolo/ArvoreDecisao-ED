#ifndef TREE_H
#define TREE_H

#include <stdbool.h>

typedef unsigned int uint;

// Nó básico de árvore binária para territórios
typedef struct Node {
    uint id;                  // identificador simples do nó
    char *territorioName;     // nome do território
    struct Node *esq;         // filho esquerdo
    struct Node *dir;         // filho direito
    int estaOcupado;          // 0 = livre; >0 pode representar jogador
    bool ehFolha;             // true se é folha (território final)
} Node;

// Create a leaf node (final guess)
Node *tree_create_leaf(const char *text);

// Create an internal territory node
Node *tree_create_question(const char *territorioName);

// Attach children (esquerda e direita)
void tree_set_children(Node *parent, Node *esq, Node *dir);

// Utility: is node a leaf?
bool tree_is_leaf(const Node *n);

// Free the whole tree (recursively)
void tree_free(Node *root);

// Optional: serialize/deserialize to file for persistence
bool tree_save_to_file(const char *path, const Node *root);
Node *tree_load_from_file(const char *path);

// Constrói uma árvore inicial com exatamente 15 nós (7 internos, 8 folhas)
Node *tree_build_initial_15(void);

// Conta número de nós
unsigned int tree_count_nodes(const Node *root);

#endif // TREE_H
