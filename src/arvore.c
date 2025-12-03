#include "arvore.h"
#include "utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Contador global para IDs únicos
static int id_counter = 0;

Territorio* criar_no_recursivo(int nivel_atual, int max_nivel) {
    Territorio *novo = (Territorio*) malloc(sizeof(Territorio));
    novo->id = ++id_counter;
    novo->nivel = nivel_atual;
    novo->ocupado = 0;
    novo->visitado = 0;
    novo->tem_farm = 0; // ///ADDED: Inicializa sem farm ///
    novo->esq = NULL;
    novo->dir = NULL;

    // Atributos aleatórios
    novo->xp = rng_int(0, 3);
    novo->pureza_base = rng_int(1, 3);
    
    // Tipo de território
    int r_tipo = rng_int(0, 100);
    if (r_tipo < 50) novo->tipo = NORMAL;
    else if (r_tipo < 80) novo->tipo = RECURSO;
    else novo->tipo = BATALHA;

    // Vestígio
    int r_vest = rng_int(0, 100);
    if (r_vest < 60) novo->vestigio = NEUTRO;
    else if (r_vest < 80) novo->vestigio = ATAQUE;
    else novo->vestigio = DEFESA;

    // Nível mínimo para entrar neste nó (R(d))
    // Quanto menor o nível (mais perto da raiz), maior o requisito
    // Nível 3 (folhas): 0
    // Nível 2: 10
    // Nível 1: 20
    // Nível 0 (Raiz): 30
    // ///ADDED: Fórmula de nível mínimo baseada na profundidade ///
    if (nivel_atual == 3) novo->nivel_minimo = 0;
    else if (nivel_atual == 2) novo->nivel_minimo = 10;
    else if (nivel_atual == 1) novo->nivel_minimo = 20;
    else novo->nivel_minimo = 30;

    // Nome
    if (nivel_atual == 0) {
        sprintf(novo->nome, "Nucleo-X (Raiz)");
        novo->tipo = BATALHA; // Raiz é sempre batalha final
    } else {
        sprintf(novo->nome, "Setor-%d-%d", nivel_atual, novo->id);
    }

    if (nivel_atual < max_nivel) {
        novo->esq = criar_no_recursivo(nivel_atual + 1, max_nivel);
        novo->dir = criar_no_recursivo(nivel_atual + 1, max_nivel);
    }

    return novo;
}

Territorio* criar_arvore_padrao() {
    id_counter = 0;
    // Raiz é nível 0, folhas nível 3
    return criar_no_recursivo(0, 3);
}

// ///ADDED: Produção de XP da Farm baseada no nível ///
int calcular_producao_farm(int nivel) {
    // Nível 3 (Folha) = 1 XP
    // Nível 2 = 2 XP
    // Nível 1 = 3 XP
    // Nível 0 = 0 XP (Raiz não tem farm, é o objetivo)
    if (nivel == 0) return 0;
    return 4 - nivel;
}

Territorio* buscar_pai(Territorio *raiz, Territorio *alvo) {
    if (raiz == NULL || alvo == NULL) return NULL;
    
    if (raiz->esq == alvo || raiz->dir == alvo) {
        return raiz;
    }

    Territorio *res = buscar_pai(raiz->esq, alvo);
    if (res != NULL) return res;

    return buscar_pai(raiz->dir, alvo);
}

// Helper para coletar nós de um nível específico
void coletar_nos_nivel(Territorio *raiz, int nivel_alvo, Territorio **lista, int *qtd) {
    if (raiz == NULL) return;
    
    if (raiz->nivel == nivel_alvo) {
        lista[(*qtd)++] = raiz;
        return;
    }
    
    coletar_nos_nivel(raiz->esq, nivel_alvo, lista, qtd);
    coletar_nos_nivel(raiz->dir, nivel_alvo, lista, qtd);
}

Territorio* buscar_vizinho(Territorio *raiz, Territorio *atual, int direcao) {
    if (raiz == NULL || atual == NULL) return NULL;
    
    // Array temporário para armazenar nós do nível (max 8 nós no nível 3)
    Territorio *nos[16]; 
    int qtd = 0;
    
    coletar_nos_nivel(raiz, atual->nivel, nos, &qtd);
    
    int index = -1;
    for (int i = 0; i < qtd; i++) {
        if (nos[i] == atual) {
            index = i;
            break;
        }
    }
    
    if (index == -1) return NULL; // Não encontrado (erro?)
    
    int novo_index = index + direcao;
    if (novo_index >= 0 && novo_index < qtd) {
        return nos[novo_index];
    }
    
    return NULL; // Sem vizinho nessa direção
}

void liberar_arvore(Territorio *raiz) {
    if (raiz == NULL) return;
    liberar_arvore(raiz->esq);
    liberar_arvore(raiz->dir);
    free(raiz);
}

void imprimir_arvore(Territorio *raiz, int nivel) {
    if (raiz == NULL) return;
    
    for (int i = 0; i < nivel; i++) printf("  ");
    printf("|- %s (Lvl %d, XP req: %d) [%s]\n", 
        raiz->nome, raiz->nivel, raiz->nivel_minimo, 
        raiz->ocupado == 0 ? "Livre" : (raiz->ocupado == 1 ? "P1" : "P2"));
    
    imprimir_arvore(raiz->esq, nivel + 1);
    imprimir_arvore(raiz->dir, nivel + 1);
}
