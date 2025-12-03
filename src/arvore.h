#ifndef ARVORE_H
#define ARVORE_H

typedef enum { NORMAL, RECURSO, BATALHA } TipoTerritorio;
typedef enum { NEUTRO, ATAQUE, DEFESA } TipoVestigio;

typedef struct Territorio {
    char nome[40];
    TipoTerritorio tipo;
    int ocupado; // 0 = livre, 1 = jogador1, 2 = jogador2
    struct Territorio *esq, *dir;
    int xp; // 0..3 aleatório
    int nivel; // profundidade d (0..3)
    int nivel_minimo; // R(d)
    TipoVestigio vestigio;
    int pureza_base; // 1..3
    int visitado; // flag
    int id; // Identificador único para facilitar debug
    int tem_farm; // 0 = sem farm, 1 = farm do player 1, 2 = farm do player 2
} Territorio;

// Cria a árvore completa de altura 3 (15 nós)
Territorio* criar_arvore_padrao();

// Calcula XP gerado por uma farm neste nível
int calcular_producao_farm(int nivel);

// Busca o pai de um nó alvo a partir da raiz
Territorio* buscar_pai(Territorio *raiz, Territorio *alvo);

// ///ADDED: Busca vizinho lateral (mesmo nível) ///
// direcao: -1 (esquerda), 1 (direita)
Territorio* buscar_vizinho(Territorio *raiz, Territorio *atual, int direcao);

// Libera a memória da árvore
void liberar_arvore(Territorio *raiz);

// Função auxiliar para imprimir a árvore (debug)
void imprimir_arvore(Territorio *raiz, int nivel);

#endif
