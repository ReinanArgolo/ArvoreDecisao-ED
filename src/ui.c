#include "ui.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Preenche o array nodes com ponteiros para os nós da árvore (índice 1 a 15)
void flatten_tree(Territorio *raiz, Territorio **nodes, int idx) {
    if (raiz == NULL || idx > 15) return;
    nodes[idx] = raiz;
    flatten_tree(raiz->esq, nodes, 2 * idx);
    flatten_tree(raiz->dir, nodes, 2 * idx + 1);
}

void print_node_ui(Territorio *t, Player *p1, Player *p2, Player *atual) {
    if (t == NULL) {
        printf("      "); // Espaço vazio equivalente a "[XX] "
        return;
    }

    char *cor = RST;
    if (t->ocupado == p1->id) cor = AZUL;
    else if (t->ocupado == p2->id) cor = VERM;
    else if (t->nivel == 0) cor = AMA; // Raiz

    char extra = ' ';
    char *cor_extra = RST; // Cor padrão para o símbolo extra

    if (t->tem_farm) {
        extra = '$';
        // Define a cor do $ baseada no dono da farm
        if (t->tem_farm == p1->id) cor_extra = AZUL;
        else if (t->tem_farm == p2->id) cor_extra = VERM;
    }
    else if (t->nivel == 0) extra = '+'; // Coroa/Raiz

    // Se o jogador atual está aqui, destaca
    int is_here = (atual->pos == t);

    // Se o nó já tem cor (ocupado), o extra herdaria a cor.
    // Mas queremos garantir que o $ tenha a cor do dono da farm.
    // Se o nó estiver ocupado pelo dono da farm, tudo é azul/vermelho.
    // Se o nó estiver livre (mas com farm?), ou ocupado por outro (conflito?), a cor do $ deve prevalecer.
    // Nota: Pela lógica do jogo, se tem farm, o nó deve ser do dono ou foi recém tomado?
    // Assumindo que a farm persiste, forçar a cor do extra.

    if (is_here) {
        printf("%s[%02d%s%c%s]<%s", cor, t->id, cor_extra, extra, cor, RST);
    } else {
        printf("%s[%02d%s%c%s]%s ", cor, t->id, cor_extra, extra, cor, RST);
    }
}

void desenhar_interface(Territorio *raiz, Player *p1, Player *p2, Player *atual, int rodada, char *log_msg) {
    limpar_tela();

    // 1. Cabeçalho
    printf("================================================================================\n");
    printf("                        CODEBATTLE: A CONQUISTA DO NUCLEO-X                     \n");
    printf("================================================================================\n");
    
    // Stats P1 e P2
    char *cor_p1 = (p1->id == 1) ? AZUL : VERM;
    char *cor_p2 = (p2->id == 2) ? VERM : AZUL;

    printf(" [ %s%s (J%d)%s ] \t\t\t|          [ %s%s (J%d)%s ]\n", 
        cor_p1, p1->nome, p1->id, RST, 
        cor_p2, p2->nome, p2->id, RST);
    
    printf(" HP: %03d | XP: %03d | ATK: %02d | DEF: %02d\t| HP: %03d | XP: %03d | ATK: %02d | DEF: %02d\n",
        p1->V, p1->Xp, p1->Aatk, p1->Adef,
        p2->V, p2->Xp, p2->Aatk, p2->Adef);
        
    printf(" Local: No [%02d] (Nivel %d)\t\t\t| Local: No [%02d] (Nivel %d)\n",
        p1->pos->id, p1->pos->nivel, p2->pos->id, p2->pos->nivel);
        
    printf(" --------------------------------------------------------------------------------\n\n");

    // 2. Mapa (Árvore)
    Territorio *nodes[16];
    for(int i=0; i<16; i++) nodes[i] = NULL;
    flatten_tree(raiz, nodes, 1);

    // Nível 0
    printf("                                     ");
    print_node_ui(nodes[1], p1, p2, atual);
    printf("\n");
    printf("                                    /     \\\n");
    printf("                          _________/       \\_________\n");
    printf("                         /                           \\\n");

    // Nível 1
    printf("                     ");
    print_node_ui(nodes[2], p1, p2, atual);
    printf("                           ");
    print_node_ui(nodes[3], p1, p2, atual);
    printf("\n");
    printf("                  /        \\                        /      \\\n");

    // Nível 2
    printf("              ");
    print_node_ui(nodes[4], p1, p2, atual);
    printf("      ");
    print_node_ui(nodes[5], p1, p2, atual);
    printf("                ");
    print_node_ui(nodes[6], p1, p2, atual);
    printf("      ");
    print_node_ui(nodes[7], p1, p2, atual);
    printf("\n");
    printf("             /   \\     /      \\               /    \\        /    \\\n");

    // Nível 3
    printf("          ");
    print_node_ui(nodes[8], p1, p2, atual);
    printf("  ");
    print_node_ui(nodes[9], p1, p2, atual);
    print_node_ui(nodes[10], p1, p2, atual);
    print_node_ui(nodes[11], p1, p2, atual);
    printf("        ");
    print_node_ui(nodes[12], p1, p2, atual);
    print_node_ui(nodes[13], p1, p2, atual);
    printf("  ");
    print_node_ui(nodes[14], p1, p2, atual);
    print_node_ui(nodes[15], p1, p2, atual);
    printf("\n\n");

    printf(" --------------------------------------------------------------------------------\n");
    printf(" [LOG DE EVENTOS]\n");
    if (log_msg && strlen(log_msg) > 0) {
        printf(" > %s\n", log_msg);
    } else {
        printf(" > Aguardando acoes...\n");
    }
    printf(" > Turno de %s (Rodada %d)\n", atual->nome, rodada);
    printf("\n");
}

void animacao_batalha(Player *atk, Player *def, int pc_atk, int pc_def, int venceu) {
    printf("\n%s!!! BATALHA INICIADA EM [%02d] !!!%s\n", VERM, atk->pos->id, RST);
    printf("%s (ATK) vs %s (DEF)\n", atk->nome, def->nome);
    
    printf("%s rolou PC de Ataque: %d\n", atk->nome, pc_atk);
    printf("%s rolou PC de Defesa: %d\n", def->nome, pc_def);
    
    if (venceu) {
        printf("\n%s>> VITORIA DE %s! <<%s\n", VERD, atk->nome, RST);
        printf("%s foi enviado de volta ao SPAWN!\n", def->nome);
    } else {
        printf("\n%s>> DEFESA BEM SUCEDIDA DE %s! <<%s\n", AZUL, def->nome, RST);
        printf("%s falhou e voltou ao SPAWN!\n", atk->nome);
    }
    
    printf("\n[Pressione ENTER para continuar...]");
    limpar_buffer();
    getchar();
}
