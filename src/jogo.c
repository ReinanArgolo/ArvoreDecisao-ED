#include "jogo.h"
#include "utils.h"
#include <stdio.h>
#include <math.h>
#include <string.h>

// ///ADDED: Função auxiliar para calcular bônus baseado em XP ///
int calcular_bonus_xp(int xp) {
    return (int)(xp * ((double)OMEGA_XP_NUM / OMEGA_XP_DEN));
}

void aplicar_vestigio(Player *p, Territorio *t) {
    if (t->visitado) return; // Só aplica na primeira visita

    int b_base = t->pureza_base;
    int ganho = b_base + (p->Xp / K_AMORTECIMENTO);
    
    printf(">> Vestigio encontrado: ");
    switch(t->vestigio) {
        case ATAQUE:
            printf("ATAQUE! +%d Atk\n", ganho);
            p->Aatk += ganho;
            break;
        case DEFESA:
            printf("DEFESA! +%d Def\n", ganho);
            p->Adef += ganho;
            break;
        case NEUTRO:
            printf("NEUTRO! +%d Vitalidade\n", ganho);
            p->V += ganho;
            break;
    }
    t->visitado = 1;
}

void iniciar_batalha(Player *atk, Player *def) {
    printf("\n!!! BATALHA INICIADA !!!\n");
    printf("%s (Atk) vs %s (Def)\n", atk->nome, def->nome);

    // ///ADDED: Novas Fórmulas de Batalha (Solicitado pelo usuário) ///
    // PC_atk = Aatk + (Adef * 0.5) + (0.5 * Xp)
    // PC_def = Adef + (Aatk * 0.5) + (0.5 * Xp) + DELTA_DEFESA
    
    double omega = (double)OMEGA_XP_NUM / OMEGA_XP_DEN;
    
    double pc_atk_val = atk->Aatk + (atk->Adef * 0.5) + (omega * atk->Xp);
    double pc_def_val = def->Adef + (atk->Aatk * 0.5) + (omega * def->Xp) + DELTA_DEFESA;

    // Arredondando para inteiro para comparação simples, ou mantendo double?
    // O relatório original usava inteiros, mas a fórmula tem 0.5. Vamos usar inteiros com cast.
    int pc_atk = (int)pc_atk_val;
    int pc_def = (int)pc_def_val;

    printf("PC Atacante: %d (%.1f) | PC Defensor: %d (%.1f)\n", pc_atk, pc_atk_val, pc_def, pc_def_val);

    if (pc_atk > pc_def) {
        printf(">> %s VENCEU a batalha!\n", atk->nome);
        
        // Consequências
        // ///ADDED: Perdedor volta para o spawn ///
        printf("%s foi derrotado e voltou para o SPAWN!\n", def->nome);
        def->pos->ocupado = 0; // Libera atual
        
        // Limpa histórico pois voltou ao início
        liberar_pilha(&def->historico);
        
        def->pos = def->spawn;
        def->pos->ocupado = def->id; // Ocupa spawn
        push(&def->historico, def->pos); // Reinicia histórico

        // Atacante ganha XP (fração do perdedor)
        // "O vencedor absorve uma fração do Xp do perdedor"
        int xp_ganho = (def->Xp / 2) + 5;
        atk->Xp += xp_ganho;
        
        // "ganha um incremento imediato em Aatk ou Adef"
        if (rng_int(0, 1) == 0) {
            atk->Aatk += 1;
            printf("%s ganhou %d XP e +1 Atk.\n", atk->nome, xp_ganho);
        } else {
            atk->Adef += 1;
            printf("%s ganhou %d XP e +1 Def.\n", atk->nome, xp_ganho);
        }

    } else {
        printf(">> %s DEFENDEU com sucesso!\n", def->nome);
        
        // ///ADDED: Atacante perdeu e volta para o spawn ///
        printf("%s falhou no ataque e voltou para o SPAWN!\n", atk->nome);
        
        // Atacante já estava no nó anterior (não moveu), mas agora deve voltar ao spawn
        // Nota: A função mover_para_pai ainda não tinha movido o atacante.
        // Ele estava em atk->pos. Agora ele vai para atk->spawn.
        atk->pos->ocupado = 0;
        liberar_pilha(&atk->historico);
        atk->pos = atk->spawn;
        atk->pos->ocupado = atk->id;
        push(&atk->historico, atk->pos);

        // Defensor ganha XP pela defesa bem-sucedida.
        def->Xp += 5;
        printf("%s ganhou 5 XP pela defesa.\n", def->nome);
    }
}

// ///ADDED: Coleta XP das Farms recursivamente ///
void coletar_xp_farms(Territorio *raiz, Player *p) {
    if (raiz == NULL) return;
    
    if (raiz->tem_farm == p->id) {
        int producao = calcular_producao_farm(raiz->nivel);
        p->Xp += producao;
        // printf("Farm em %s produziu %d XP.\n", raiz->nome, producao); // Verbose demais?
    }
    
    coletar_xp_farms(raiz->esq, p);
    coletar_xp_farms(raiz->dir, p);
}

// ///ADDED: Gerencia construção/destruição de Farms ///
void gerenciar_farm(Player *p) {
    if (p->pos->tem_farm == p->id) {
        printf("Voce ja tem uma farm aqui.\n");
        return;
    }
    
    if (p->pos->tem_farm == 0) {
        printf("Construindo farm em %s...\n", p->pos->nome);
        p->pos->tem_farm = p->id;
        printf("Farm construida! Produzira %d XP por turno.\n", calcular_producao_farm(p->pos->nivel));
    } else {
        // Farm inimiga
        printf("Destruindo farm inimiga em %s...\n", p->pos->nome);
        p->pos->tem_farm = 0; // Destrói. Próximo turno pode construir.
        printf("Farm destruida!\n");
    }
}

// ///ADDED: Compra de Atributos ///
void comprar_atributos(Player *p, int *compras_feitas) {
    if (*compras_feitas >= 2) {
        printf("Limite de 2 compras por turno atingido.\n");
        return;
    }
    
    int custo = 5; // Custo fixo por enquanto
    if (p->Xp < custo) {
        printf("XP insuficiente (%d). Custo: %d.\n", p->Xp, custo);
        return;
    }
    
    printf("Comprar: 1. Ataque (+1) | 2. Defesa (+1) | 0. Cancelar\n");
    int op;
    scanf("%d", &op);
    
    if (op == 1) {
        p->Xp -= custo;
        p->Aatk++;
        (*compras_feitas)++;
        printf("Ataque aumentado! XP restante: %d\n", p->Xp);
    } else if (op == 2) {
        p->Xp -= custo;
        p->Adef++;
        (*compras_feitas)++;
        printf("Defesa aumentada! XP restante: %d\n", p->Xp);
    }
}

// ///ADDED: Movimento Lateral ///
int mover_lateralmente(Player *p, Territorio *raiz, Player *adversario, int direcao) {
    Territorio *vizinho = buscar_vizinho(raiz, p->pos, direcao);
    
    if (vizinho == NULL) {
        printf("Nao ha caminho para essa direcao.\n");
        return 0;
    }

    printf("%s tenta mover lateralmente para %s...\n", p->nome, vizinho->nome);

    // Verificar ocupação
    if (vizinho->ocupado != 0 && vizinho->ocupado != p->id) {
        // Ocupado por adversário
        iniciar_batalha(p, adversario);
        
        // Se após a batalha o vizinho ainda estiver ocupado pelo adversário, o movimento falhou
        if (vizinho->ocupado == adversario->id) {
            return 0;
        }
        // Se venceu, o vizinho agora deve estar livre (adversário recuou)
    }

    // Movimento bem sucedido
    p->pos->ocupado = 0; // Libera nó atual
    
    // Salvar histórico? 
    // O prompt diz "O Pdefensor é expulso do nó, sendo forçado a recuar para sua posição anterior (utilizando a pilha de histórico de movimento)."
    // Se eu mover lateralmente, devo empilhar? Sim, para poder recuar.
    push(&p->historico, p->pos);
    
    // Mover
    p->pos = vizinho;
    p->pos->ocupado = p->id;
    
    printf("Movimento lateral realizado com sucesso para %s!\n", vizinho->nome);
    
    // Aplicar vestígio
    aplicar_vestigio(p, vizinho);

    return 1;
}

int mover_para_pai(Player *p, Territorio *raiz, Player *adversario) {
    Territorio *pai = buscar_pai(raiz, p->pos);
    
    if (pai == NULL) {
        printf("Voce ja esta no topo ou erro na arvore.\n");
        return 0;
    }

    // Verificar restrição de nível R(d)
    if (p->Xp < pai->nivel_minimo) {
        printf("Movimento vetado! XP insuficiente (%d/%d) para entrar em %s.\n", 
               p->Xp, pai->nivel_minimo, pai->nome);
        return 0;
    }

    printf("%s tenta mover para %s...\n", p->nome, pai->nome);

    // Verificar ocupação
    if (pai->ocupado != 0 && pai->ocupado != p->id) {
        // Ocupado por adversário
        iniciar_batalha(p, adversario);
        
        // Se após a batalha o pai ainda estiver ocupado pelo adversário, o movimento falhou
        if (pai->ocupado == adversario->id) {
            return 0;
        }
        // Se venceu, o pai agora deve estar livre (adversário recuou)
    }

    // ///ADDED: Custo de XP para subir ///
    p->Xp -= pai->nivel_minimo;
    printf("Gasto de %d XP para subir de nivel.\n", pai->nivel_minimo);

    // Movimento bem sucedido
    // Atualizar flags
    p->pos->ocupado = 0; // Libera nó atual
    
    // Salvar histórico
    push(&p->historico, p->pos);
    
    // Mover
    p->pos = pai;
    p->pos->ocupado = p->id;
    
    printf("Movimento realizado com sucesso para %s!\n", pai->nome);
    
    // Aplicar vestígio
    aplicar_vestigio(p, pai);

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
        printf("\n*** %s CONQUISTOU O NUCLEO-X! VITORIA! ***\n", p->nome);
        return 1;
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

    while (jogo_ativo) {
        limpar_tela(); // ADDED: Limpar tela a cada rodada
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
            printf("\n\n>>> RODADA %d - Turno de %s <<<\n", rodada, atual->nome);
            imprimir_status(atual);

            // Verificar vitória antes do turno
            if (verificar_vitoria(atual, raiz)) {
                jogo_ativo = 0;
                turno_ativo = 0;
                break;
            }

            int escolha = 0;

            if (modo_demo) {
                // Heurística simples para o modo demo
                // 1. Tentar subir se tiver XP suficiente
                Territorio *pai = buscar_pai(raiz, atual->pos);
                if (pai != NULL && atual->Xp >= pai->nivel_minimo) {
                    escolha = 1; // Tentar subir
                } else if (atual->pos->tipo == RECURSO && !atual->pos->visitado) {
                    escolha = 2; // Coletar recurso
                } else {
                    // Se não pode subir e não tem recurso, tenta explorar ou passar
                    // Se tiver item de energia e vida baixa, usa
                    if (atual->V < 5 && consumir_item(&atual->inv, ENERGIA)) {
                        escolha = 3;
                    } else {
                        // Tentar mover lateralmente para achar XP
                        // Alterna aleatoriamente entre esq/dir
                        if (rng_int(0, 1) == 0) escolha = 5;
                        else escolha = 6;
                    }
                }
                // Fallback randomico para não travar
                if (escolha == 0) escolha = rng_int(1, 6);
                
                printf("Modo Demo escolheu: %d\n", escolha);
            } else {
                // Menu de ações
                printf("Acoes:\n");
                printf("1. Subir para o pai (Tentar conquistar)\n");
                printf("2. Explorar territorio atual (Buscar recursos)\n");
                printf("3. Usar item\n");
                printf("4. Passar a vez\n");
                printf("5. Mover para Esquerda (Lateral)\n");
                printf("6. Mover para Direita (Lateral)\n");
                printf("7. Gerenciar Farm (Construir/Destruir)\n");
                printf("8. Comprar Atributos (Loja)\n");
                printf("Escolha: ");

                if (scanf("%d", &escolha) != 1) {
                    limpar_buffer();
                    escolha = 4;
                }
            }

            switch(escolha) {
                case 1:
                    mover_para_pai(atual, raiz, outro);
                    turno_ativo = 0;
                    break;
                case 2:
                    if (!atual->pos->visitado) {
                        if (atual->pos->tipo == RECURSO) {
                            Item novo_item;
                            int r = rng_int(0, 2);
                            if (r == 0) { novo_item.tipo = ARMA; strcpy(novo_item.nome, "Espada de Bits"); novo_item.valor_bonus = 2; }
                            else if (r == 1) { novo_item.tipo = ESCUDO; strcpy(novo_item.nome, "Firewall"); novo_item.valor_bonus = 2; }
                            else { novo_item.tipo = ENERGIA; strcpy(novo_item.nome, "Cafeina"); novo_item.valor_bonus = 5; }
                            
                            adicionar_item(&atual->inv, novo_item);
                            printf("Voce encontrou: %s!\n", novo_item.nome);
                        } else {
                            printf("Explorou o local, mas nao havia itens.\n");
                        }
                        atual->pos->visitado = 1;
                        atual->Xp += 2;
                        printf("Ganhou +2 XP pela exploracao.\n");
                    } else {
                        printf("Local ja explorado. Nada novo aqui.\n");
                    }
                    turno_ativo = 0;
                    break;
                case 3:
                    if (modo_demo) {
                        if (consumir_item(&atual->inv, ENERGIA)) { atual->V += 5; printf("Demo usou Energia.\n"); }
                        else if (consumir_item(&atual->inv, ARMA)) { atual->Aatk += 2; printf("Demo usou Arma.\n"); }
                        else if (consumir_item(&atual->inv, ESCUDO)) { atual->Adef += 2; printf("Demo usou Escudo.\n"); }
                        else printf("Demo tentou usar item mas inventario vazio.\n");
                        turno_ativo = 0;
                    } else {
                        printf("Qual item usar? (1-ARMA, 2-ESCUDO, 3-ENERGIA): ");
                        int tipo_item;
                        scanf("%d", &tipo_item);
                        int bonus = 0;
                        if (tipo_item == 1) {
                            bonus = consumir_item(&atual->inv, ARMA);
                            if (bonus) { atual->Aatk += bonus; printf("Ataque aumentado em %d!\n", bonus); }
                        } else if (tipo_item == 2) {
                            bonus = consumir_item(&atual->inv, ESCUDO);
                            if (bonus) { atual->Adef += bonus; printf("Defesa aumentada em %d!\n", bonus); }
                        } else if (tipo_item == 3) {
                            bonus = consumir_item(&atual->inv, ENERGIA);
                            if (bonus) { atual->V += bonus; printf("Vitalidade recuperada em %d!\n", bonus); }
                        }
                        if (bonus == 0) printf("Item nao disponivel.\n");
                        else turno_ativo = 0;
                    }
                    break;
                case 4:
                    printf("%s passou a vez.\n", atual->nome);
                    turno_ativo = 0;
                    break;
                case 5:
                    mover_lateralmente(atual, raiz, outro, -1);
                    turno_ativo = 0;
                    break;
                case 6:
                    mover_lateralmente(atual, raiz, outro, 1);
                    turno_ativo = 0;
                    break;
                case 7:
                    gerenciar_farm(atual);
                    turno_ativo = 0;
                    break;
                case 8:
                    comprar_atributos(atual, &compras_neste_turno);
                    break;
                default:
                    printf("Opcao invalida.\n");
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
            printf("\n[Pressione ENTER para continuar...]");
            limpar_buffer(); // Garante que o buffer esteja limpo antes de esperar
            getchar(); // Espera o ENTER
        }
    }

    liberar_fila(&fila_turnos);
}
