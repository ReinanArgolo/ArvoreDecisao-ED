#ifndef TREE_H
#define TREE_H

#include <stdbool.h>

// Basic binary tree node for decision tree
typedef struct Node {
    char *question;            // Text to ask or final guess
    struct Node *yes;          // Yes branch
    struct Node *no;           // No branch
    bool isLeaf;               // True if leaf (final guess)
} Node;

// Create a leaf node (final guess)
Node *tree_create_leaf(const char *text);

// Create an internal question node with empty branches initially
Node *tree_create_question(const char *question);

// Attach children to a question node
void tree_set_children(Node *parent, Node *yesChild, Node *noChild);

// Utility: is node a leaf?
bool tree_is_leaf(const Node *n);

// Free the whole tree (recursively)
void tree_free(Node *root);

// Optional: serialize/deserialize to file for persistence
bool tree_save_to_file(const char *path, const Node *root);
Node *tree_load_from_file(const char *path);

#endif // TREE_H
