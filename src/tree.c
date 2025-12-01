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

Node *tree_create_leaf(const char *text) {
    Node *n = (Node*)calloc(1, sizeof(Node));
    if (!n) return NULL;
    n->question = dupstr(text);
    n->isLeaf = true;
    return n;
}

Node *tree_create_question(const char *question) {
    Node *n = (Node*)calloc(1, sizeof(Node));
    if (!n) return NULL;
    n->question = dupstr(question);
    n->isLeaf = false;
    return n;
}

void tree_set_children(Node *parent, Node *yesChild, Node *noChild) {
    if (!parent) return;
    parent->yes = yesChild;
    parent->no = noChild;
}

bool tree_is_leaf(const Node *n) {
    return n && n->isLeaf;
}

void tree_free(Node *root) {
    if (!root) return;
    tree_free(root->yes);
    tree_free(root->no);
    free(root->question);
    free(root);
}

// Simple line-based serialization: each node as: type|text
// type: Q for question, L for leaf. Preorder traversal.
static bool save_preorder(FILE *f, const Node *n) {
    if (!n) {
        fputs("#\n", f); // marker for null
        return true;
    }
    fputc(n->isLeaf ? 'L' : 'Q', f);
    fputc('|', f);
    if (n->question) fputs(n->question, f);
    fputc('\n', f);
    if (!n->isLeaf) {
        if (!save_preorder(f, n->yes)) return false;
        if (!save_preorder(f, n->no)) return false;
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
    // trim newline
    size_t len = strlen(line);
    if (len && line[len-1] == '\n') line[len-1] = '\0';
    if (strcmp(line, "#") == 0) return NULL; // null marker
    if (line[0] != 'L' && line[0] != 'Q') return NULL;
    char *sep = strchr(line, '|');
    if (!sep) return NULL;
    char type = line[0];
    const char *text = sep + 1;
    if (type == 'L') {
        return tree_create_leaf(text);
    } else {
        Node *q = tree_create_question(text);
        q->yes = load_preorder(f);
        q->no = load_preorder(f);
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
