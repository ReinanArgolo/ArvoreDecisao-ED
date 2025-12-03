#ifndef LISTA_H
#define LISTA_H

typedef enum { ARMA, ESCUDO, ENERGIA } TipoItem;

typedef struct Item {
    char nome[30];
    int valor_bonus;
    TipoItem tipo;
} Item;

typedef struct NoLista {
    Item item;
    struct NoLista *prox;
} NoLista;

typedef struct {
    NoLista *inicio;
    int tamanho;
} Lista;

void inicializar_lista(Lista *l);
void adicionar_item(Lista *l, Item item);
// Consome um item do tipo especificado, retornando o valor do bônus. Retorna 0 se não tiver.
int consumir_item(Lista *l, TipoItem tipo);
void liberar_lista(Lista *l);
void listar_itens(Lista *l);

#endif
