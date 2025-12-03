#include "jogo.h"
#include "utils.h"
#include "ui.h"
#include <stdio.h>
#include <math.h>
#include <string.h>

// Helper to append to log
void log_append(char *buffer, const char *msg) {
    if (buffer == NULL) return;
    size_t len = strlen(buffer);
    if (len + strlen(msg) < 255) {
        strcat(buffer, msg);
    }
}

// ///ADDED: Função auxiliar para calcular bônus baseado em XP ///
int calcular_bonus_xp(int xp) {
    return (int)(xp * ((double)OMEGA_XP_NUM / OMEGA_XP_DEN));
}

void aplicar_vestigio(Player *p, Territorio *t, char *log_buffer) {
    if (t->visitado) return; // Só aplica na primeira visita

    int b_base = t->pureza_base;
    int ganho = b_base + (p->Xp / K_AMORTECIMENTO);
    
    char msg[100];
    sprintf(msg, " Vestigio: ");
    log_append(log_buffer, msg);

    switch(t->vestigio) {
        case ATAQUE:
            sprintf(msg, "ATAQUE! +%d Atk. ", ganho);
            p->Aatk += ganho;
            break;
        case DEFESA:
            sprintf(msg, "DEFESA! +%d Def. ", ganho);
            p->Adef += ganho;
            break;
        case NEUTRO:
            sprintf(msg, "NEUTRO! +%d Vit. ", ganho);
            p->V += ganho;
            break;
    }
    log_append(log_buffer, msg);
    t->visitado = 1;
}

void iniciar_batalha(Player *atk, Player *def) {
    // ///ADDED: Novas Fórmulas de Batalha (Solicitado pelo usuário) ///
    double omega = (double)OMEGA_XP_NUM / OMEGA_XP_DEN;
    
    double pc_atk_val = atk->Aatk + (atk->Adef * 0.5) + (omega * atk->Xp);
    double pc_def_val = def->Adef + (atk->Aatk * 0.5) + (omega * def->Xp) + DELTA_DEFESA;

    int pc_atk = (int)pc_atk_val;
    int pc_def = (int)pc_def_val;
    
    // Adiciona fator aleatório (dado d6)
    int dado_atk = rng_int(1, 6);
    int dado_def = rng_int(1, 6);
    
    pc_atk += dado_atk;
    pc_def += dado_def;

    int venceu = (pc_atk > pc_def);

    // Chama animação visual
    animacao_batalha(atk, def, pc_atk, pc_def, venceu);

    if (venceu) {
        // Consequências
        def->pos->ocupado = 0; // Libera atual
        liberar_pilha(&def->historico);
        def->pos = def->spawn;
        def->pos->ocupado = def->id; // Ocupa spawn
        push(&def->historico, def->pos); // Reinicia histórico

        int xp_ganho = (def->Xp / 2) + 5;
        atk->Xp += xp_ganho;
        
        if (rng_int(0, 1) == 0) {
            atk->Aatk += 1;
        } else {
            atk->Adef += 1;
        }

    } else {
        // Atacante perdeu
        atk->pos->ocupado = 0;
        liberar_pilha(&atk->historico);
        atk->pos = atk->spawn;
        atk->pos->ocupado = atk->id;
        push(&atk->historico, atk->pos);

        def->Xp += 5;
    }
}

// ///ADDED: Calcula produção total de farms do jogador ///
int calcular_total_farms(Territorio *raiz, int player_id) {
    if (raiz == NULL) return 0;
    
    int total = 0;
    if (raiz->tem_farm == player_id) {
        total += calcular_producao_farm(raiz->nivel);
    }
    
    total += calcular_total_farms(raiz->esq, player_id);
    total += calcular_total_farms(raiz->dir, player_id);
    
    return total;
}

// ///ADDED: Coleta XP das Farms recursivamente ///
void coletar_xp_farms(Territorio *raiz, Player *p) {
    if (raiz == NULL) return;
    
    if (raiz->tem_farm == p->id) {
        int producao = calcular_producao_farm(raiz->nivel);
        p->Xp += producao;
    }
    
    coletar_xp_farms(raiz->esq, p);
    coletar_xp_farms(raiz->dir, p);
}

// ///ADDED: Gerencia construção/destruição de Farms ///
void gerenciar_farm(Player *p, char *log_buffer) {
    if (p->pos->tem_farm == p->id) {
        sprintf(log_buffer, "Voce ja tem uma farm aqui.");
        return;
    }
    
    if (p->pos->tem_farm == 0) {
        p->pos->tem_farm = p->id;
        sprintf(log_buffer, "Farm construida em %s! (+%d XP/turno)", p->pos->nome, calcular_producao_farm(p->pos->nivel));
    } else {
        // Farm inimiga
        p->pos->tem_farm = 0; // Destrói. Próximo turno pode construir.
        sprintf(log_buffer, "Farm inimiga destruida em %s!", p->pos->nome);
    }
}

// ///ADDED: Compra de Atributos ///
void comprar_atributos(Player *p, int *compras_feitas, char *log_buffer) {
    if (*compras_feitas >= 2) {
        sprintf(log_buffer, "Limite de 2 compras por turno atingido.");
        return;
    }
    
    int custo = 5; // Custo fixo por enquanto
    if (p->Xp < custo) {
        sprintf(log_buffer, "XP insuficiente (%d). Custo: %d.", p->Xp, custo);
        return;
    }
    
    printf("\nComprar: 1. Ataque (+1) | 2. Defesa (+1) | 0. Cancelar\nEscolha: ");
    int op;
    scanf("%d", &op);
    
    if (op == 1) {
        p->Xp -= custo;
        p->Aatk++;
        (*compras_feitas)++;
        sprintf(log_buffer, "Ataque aumentado! XP restante: %d", p->Xp);
    } else if (op == 2) {
        p->Xp -= custo;
        p->Adef++;
        (*compras_feitas)++;
        sprintf(log_buffer, "Defesa aumentada! XP restante: %d", p->Xp);
    } else {
        sprintf(log_buffer, "Compra cancelada.");
    }
}

// ///ADDED: Movimento Lateral ///
int mover_lateralmente(Player *p, Territorio *raiz, Player *adversario, int direcao, char *log_buffer) {
    Territorio *vizinho = buscar_vizinho(raiz, p->pos, direcao);
    
    if (vizinho == NULL) {
        sprintf(log_buffer, "Nao ha caminho para essa direcao.");
        return 0;
    }

    // Verificar ocupação
    if (vizinho->ocupado != 0 && vizinho->ocupado != p->id) {
        // Ocupado por adversário
        iniciar_batalha(p, adversario);
        
        // Se após a batalha o vizinho ainda estiver ocupado pelo adversário, o movimento falhou
        if (vizinho->ocupado == adversario->id) {
            sprintf(log_buffer, "Ataque falhou. Voce voltou ao spawn.");
            return 0;
        }
        // Se venceu, o vizinho agora deve estar livre (adversário recuou)
    }

    // Movimento bem sucedido
    p->pos->ocupado = 0; // Libera nó atual
    
    push(&p->historico, p->pos);
    
    // Mover
    p->pos = vizinho;
    p->pos->ocupado = p->id;
    
    sprintf(log_buffer, "Moveu lateralmente para %s.", vizinho->nome);
    aplicar_vestigio(p, vizinho, log_buffer);

    return 1;
}

int mover_para_pai(Player *p, Territorio *raiz, Player *adversario, char *log_buffer) {
    Territorio *pai = buscar_pai(raiz, p->pos);
    
    if (pai == NULL) {
        sprintf(log_buffer, "Erro: Ja no topo ou arvore invalida.");
        return 0;
    }

    // Verificar restrição de nível R(d)
    if (p->Xp < pai->nivel_minimo) {
        sprintf(log_buffer, "XP insuficiente (%d/%d) para %s.", p->Xp, pai->nivel_minimo, pai->nome);
        return 0;
    }

    // Verificar ocupação
    if (pai->ocupado != 0 && pai->ocupado != p->id) {
        // Ocupado por adversário
        iniciar_batalha(p, adversario);
        
        // Se após a batalha o pai ainda estiver ocupado pelo adversário, o movimento falhou
        if (pai->ocupado == adversario->id) {
            sprintf(log_buffer, "Ataque falhou. Voce voltou ao spawn.");
            return 0;
        }
        // Se venceu, o pai agora deve estar livre (adversário recuou)
    }

    // ///ADDED: Custo de XP para subir ///
    p->Xp -= pai->nivel_minimo;

    // Movimento bem sucedido
    p->pos->ocupado = 0; // Libera nó atual
    
    push(&p->historico, p->pos);
    
    // Mover
    p->pos = pai;
    p->pos->ocupado = p->id;
    
    sprintf(log_buffer, "Subiu para %s! Custo: %d XP.", pai->nome, pai->nivel_minimo);
    aplicar_vestigio(p, pai, log_buffer);

    return 1;
}

// ///ADDED: Função para descer na árvore ///
int mover_para_filho(Player *p, Player *adversario, char *log_buffer) {
    if (p->pos->esq == NULL && p->pos->dir == NULL) {
        sprintf(log_buffer, "Voce ja esta na base da arvore (Folha).");
        return 0;
    }

    printf("\nPara onde descer?\n");
    if (p->pos->esq) printf(" [1] Esquerda (%s)\n", p->pos->esq->nome);
    if (p->pos->dir) printf(" [2] Direita (%s)\n", p->pos->dir->nome);
    printf(" [0] Cancelar\nEscolha > ");
    
    int op;
    scanf("%d", &op);
    
    Territorio *destino = NULL;
    if (op == 1) destino = p->pos->esq;
    else if (op == 2) destino = p->pos->dir;
    else {
        sprintf(log_buffer, "Movimento cancelado.");
        return 0;
    }

    if (destino == NULL) {
        sprintf(log_buffer, "Destino invalido.");
        return 0;
    }

    // Verificar ocupação
    if (destino->ocupado != 0 && destino->ocupado != p->id) {
        iniciar_batalha(p, adversario);
        if (destino->ocupado == adversario->id) {
            sprintf(log_buffer, "Ataque falhou. Voce voltou ao spawn.");
            return 0;
        }
    }

    // Movimento bem sucedido
    p->pos->ocupado = 0;
    push(&p->historico, p->pos);
    
    p->pos = destino;
    p->pos->ocupado = p->id;
    
    sprintf(log_buffer, "Desceu para %s.", destino->nome);
    aplicar_vestigio(p, destino, log_buffer);
    return 1;
}

int verificar_morte(Player *p) {
    if (p->V <= 0) {
        printf("\nXX %s MORREU! XX\n", p->nome);
        return 1;
    }
    return 0;
}

int verificar_vitoria(Player *p, Territorio *raiz) {
    if (p->pos == raiz) {
        if (p->Xp >= 100) {
            printf("\n*** %s CONQUISTOU O NUCLEO-X COM 100 XP! VITORIA SUPREMA! ***\n", p->nome);
            return 1;
        } else {
            // Opcional: Avisar que precisa de 100 XP
        }
    }
    return 0;
}

void game_loop(Territorio *raiz, Player *p1, Player *p2, int modo_demo) {
    Fila fila_turnos;
    inicializar_fila(&fila_turnos);
    
    enqueue(&fila_turnos, p1);
    enqueue(&fila_turnos, p2);

    int rodada = 1;
    int jogo_ativo = 1;
    char log_msg[256] = "Bem-vindo ao CodeBattle!";

    while (jogo_ativo) {
        Player *atual = dequeue(&fila_turnos);
        Player *outro = (atual->id == p1->id) ? p2 : p1;

        // --- NOVAS REGRAS DE XP ---
        // 1. Ganho passivo de 1 XP por turno
        atual->Xp += 1;
        // 2. Coleta de XP das Farms
        coletar_xp_farms(raiz, atual);

        int turno_ativo = 1;
        int compras_neste_turno = 0;

        while (turno_ativo) {
            desenhar_interface(raiz, p1, p2, atual, rodada, log_msg);
            
            // Reset log for next action
            log_msg[0] = '\0';

            if (verificar_vitoria(atual, raiz)) {
                jogo_ativo = 0;
                turno_ativo = 0;
                break;
            }

            int escolha = 0;

            if (modo_demo) {
                // Heurística simples para o modo demo
                Territorio *pai = buscar_pai(raiz, atual->pos);
                if (pai != NULL && atual->Xp >= pai->nivel_minimo) {
                    escolha = 1; // Tentar subir
                } else if (atual->pos->tipo == RECURSO && !atual->pos->visitado) {
                    escolha = 2; // Coletar recurso
                } else {
                    if (atual->V < 5 && consumir_item(&atual->inv, ENERGIA)) {
                        escolha = 3;
                    } else {
                        if (rng_int(0, 1) == 0) escolha = 5;
                        else escolha = 6;
                    }
                }
                if (escolha == 0) escolha = rng_int(1, 6);
                
                sprintf(log_msg, "Demo escolheu: %d", escolha);
                // Delay para demo
                // Sleep(1000); 
            } else {
                printf(" [ ACOES DISPONIVEIS ]\n");
                printf(" [1] Subir (Pai) [2] Explorar    [3] Usar Item   [4] Passar Vez\n");
                printf(" [5] Mover Esq   [6] Mover Dir   [7] Build Farm  [8] Loja\n");
                printf(" [9] Descer\n");
                printf(" ESCOLHA > ");

                if (scanf("%d", &escolha) != 1) {
                    limpar_buffer();
                    escolha = 4;
                }
            }

            switch(escolha) {
                case 1:
                    if (mover_para_pai(atual, raiz, outro, log_msg)) turno_ativo = 0;
                    else if (strlen(log_msg) == 0) sprintf(log_msg, "Acao invalida ou falha.");
                    break;
                case 2:
                    if (!atual->pos->visitado) {
                        if (atual->pos->tipo == RECURSO) {
                            Item novo_item;
                            int r = rng_int(0, 2);
                            if (r == 0) { novo_item.tipo = ARMA; strcpy(novo_item.nome, "Espada"); novo_item.valor_bonus = 2; }
                            else if (r == 1) { novo_item.tipo = ESCUDO; strcpy(novo_item.nome, "Firewall"); novo_item.valor_bonus = 2; }
                            else { novo_item.tipo = ENERGIA; strcpy(novo_item.nome, "Cafeina"); novo_item.valor_bonus = 5; }
                            
                            adicionar_item(&atual->inv, novo_item);
                            sprintf(log_msg, "Encontrou: %s!", novo_item.nome);
                        } else {
                            sprintf(log_msg, "Explorou: Nada aqui.");
                        }
                        atual->pos->visitado = 1;
                        atual->Xp += 2;
                    } else {
                        sprintf(log_msg, "Local ja explorado.");
                    }
                    turno_ativo = 0;
                    break;
                case 3:
                    if (modo_demo) {
                        if (consumir_item(&atual->inv, ENERGIA)) { atual->V += 5; sprintf(log_msg, "Demo usou Energia."); }
                        else if (consumir_item(&atual->inv, ARMA)) { atual->Aatk += 2; sprintf(log_msg, "Demo usou Arma."); }
                        else if (consumir_item(&atual->inv, ESCUDO)) { atual->Adef += 2; sprintf(log_msg, "Demo usou Escudo."); }
                        else sprintf(log_msg, "Demo sem itens.");
                        turno_ativo = 0;
                    } else {
                        printf("Qual item usar? (1-ARMA, 2-ESCUDO, 3-ENERGIA): ");
                        int tipo_item;
                        scanf("%d", &tipo_item);
                        int bonus = 0;
                        if (tipo_item == 1) {
                            bonus = consumir_item(&atual->inv, ARMA);
                            if (bonus) { atual->Aatk += bonus; sprintf(log_msg, "Ataque aumentado em %d!", bonus); }
                        } else if (tipo_item == 2) {
                            bonus = consumir_item(&atual->inv, ESCUDO);
                            if (bonus) { atual->Adef += bonus; sprintf(log_msg, "Defesa aumentada em %d!", bonus); }
                        } else if (tipo_item == 3) {
                            bonus = consumir_item(&atual->inv, ENERGIA);
                            if (bonus) { atual->V += bonus; sprintf(log_msg, "Vitalidade recuperada em %d!", bonus); }
                        }
                        if (bonus == 0) sprintf(log_msg, "Item nao disponivel.");
                        else turno_ativo = 0;
                    }
                    break;
                case 4:
                    sprintf(log_msg, "%s passou a vez.", atual->nome);
                    turno_ativo = 0;
                    break;
                case 5:
                    if (mover_lateralmente(atual, raiz, outro, -1, log_msg)) turno_ativo = 0;
                    else if (strlen(log_msg) == 0) sprintf(log_msg, "Movimento falhou.");
                    break;
                case 6:
                    if (mover_lateralmente(atual, raiz, outro, 1, log_msg)) turno_ativo = 0;
                    else if (strlen(log_msg) == 0) sprintf(log_msg, "Movimento falhou.");
                    break;
                case 7:
                    gerenciar_farm(atual, log_msg);
                    turno_ativo = 0;
                    break;
                case 8:
                    comprar_atributos(atual, &compras_neste_turno, log_msg);
                    break;
                case 9:
                    if (mover_para_filho(atual, outro, log_msg)) turno_ativo = 0;
                    else if (strlen(log_msg) == 0) sprintf(log_msg, "Movimento falhou.");
                    break;
                default:
                    sprintf(log_msg, "Opcao invalida.");
            }
        }

        // Verificações pós-turno
        if (verificar_vitoria(atual, raiz)) {
            jogo_ativo = 0;
            break;
        }

        if (verificar_morte(atual)) {
            printf("O jogo acabou. %s venceu por eliminacao!\n", outro->nome);
            jogo_ativo = 0;
            break;
        }

        enqueue(&fila_turnos, atual);
        rodada++;

        // ADDED: Pausa para leitura antes de limpar a tela
        if (jogo_ativo) {
            if (modo_demo) {
                 printf("\n[Modo Demo: Pressione ENTER]");
                 getchar();
            }
        }
    }

    liberar_fila(&fila_turnos);
}
