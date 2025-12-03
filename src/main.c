#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "arvore.h"
#include "jogador.h"
#include "jogo.h"
#include "utils.h"

// Função auxiliar para encontrar a folha mais à esquerda
Territorio* encontrar_folha_esq(Territorio *raiz) {
    if (raiz == NULL) return NULL;
    if (raiz->esq == NULL) return raiz;
    return encontrar_folha_esq(raiz->esq);
}

// Função auxiliar para encontrar a folha mais à direita
Territorio* encontrar_folha_dir(Territorio *raiz) {
    if (raiz == NULL) return NULL;
    if (raiz->dir == NULL) return raiz;
    return encontrar_folha_dir(raiz->dir);
}

void imprimir_historia() {
    printf("\n============================================================\n");
    printf(" CODE BATTLE - A CONQUISTA DA ARVORE BINARIA\n");
    printf("============================================================\n");
    printf("Em um mundo gelado e engenhoso, dois pinguins destemidos:\n");
    printf("Linus, o estrategista de bico firme, e\n");
    printf("Winderson, o explorador audaz.\n\n");
    printf("Eles descobriram lendas sobre o Nucleo-X, um antigo santuario\n");
    printf("localizado no topo de uma Arvore Ancestral.\n");
    printf("O Nucleo-X concede poder e lideranca sobre todos os territorios.\n");
    printf("Cada territorio guarda vestigios ancestrais e recursos escondidos.\n");
    printf("Linus e Winderson partem de extremos opostos da arvore e devem\n");
    printf("ascender, enfrentar guardioes e conquistar poder.\n");
    printf("Quem primeiro dominar o Nucleo-X sera coroado soberano!\n");
    printf("============================================================\n\n");
    printf("Pressione ENTER para comecar...");
    getchar();
}

int main(int argc, char *argv[]) {
    int modo_demo = 0;
    if (argc > 1 && strcmp(argv[1], "demo") == 0) {
        modo_demo = 1;
        printf(">>> MODO DEMO ATIVADO <<<\n");
    }

    inicializar_rng();
    imprimir_historia();

    printf("Gerando o mundo (Arvore Binaria de 15 nos)...\n");
    Territorio *raiz = criar_arvore_padrao();
    
    // Debug: mostrar árvore
    // imprimir_arvore(raiz, 0);

    // Definir posições iniciais
    Territorio *inicio_linus = encontrar_folha_esq(raiz);
    Territorio *inicio_winderson = encontrar_folha_dir(raiz);

    if (inicio_linus == NULL || inicio_winderson == NULL) {
        fprintf(stderr, "Erro critico: Nao foi possivel encontrar posicoes iniciais.\n");
        return 1;
    }

    printf("Criando jogadores...\n");
    Player *linus = criar_jogador(1, "Linus", inicio_linus);
    Player *winderson = criar_jogador(2, "Winderson", inicio_winderson);

    printf("Tudo pronto! O jogo vai comecar.\n");
    
    game_loop(raiz, linus, winderson, modo_demo);

    printf("\n=== FIM DE JOGO ===\n");
    printf("Limpando memoria...\n");
    
    destruir_jogador(linus);
    destruir_jogador(winderson);
    liberar_arvore(raiz);

    return 0;
}
