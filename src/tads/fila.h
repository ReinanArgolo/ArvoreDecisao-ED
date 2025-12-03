#ifndef FILA_H
#define FILA_H

// Forward declaration para evitar dependência circular
struct Player;

typedef struct NoFila {
    struct Player *player;
    struct NoFila *prox;
} NoFila;

typedef struct {
    NoFila *inicio;
    NoFila *fim;
} Fila;

void inicializar_fila(Fila *f);
void enqueue(Fila *f, struct Player *p);
struct Player* dequeue(Fila *f);
int fila_vazia(Fila *f);
void liberar_fila(Fila *f);

#endif
