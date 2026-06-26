/* Simulador da rede de distribuicao de agua - Estrutura de Dados II
   Le a rede de um ficheiro e permite analisar a sua resiliencia:
   caminho da agua, roturas, nos criticos, pontes e isolamento de vazamentos. */
#include <stdio.h>
#include <stdlib.h>
#include "grafo.h"
#include "fila.h"
#include "hash.h"
#include "analise.h"
#include "ficheiros.h"

#define FICH_ENTRADA   "dados/cidade.txt"
#define FICH_RELATORIO "dados/relatorio.txt"
#define CAP_NOS        256

/* le um numero inteiro de uma linha; devolve 0 se nao conseguir */
static int ler_inteiro(const char *prompt, int *valor) {
    char buf[128];
    printf("%s", prompt);
    if (fgets(buf, sizeof(buf), stdin) == NULL) return 0;
    return sscanf(buf, "%d", valor) == 1;
}

static void ler_palavra(const char *prompt, char *destino) {
    char buf[128];
    printf("%s", prompt);
    if (fgets(buf, sizeof(buf), stdin) == NULL) {
        destino[0] = '\0';
        return;
    }
    if (sscanf(buf, "%47s", destino) != 1) destino[0] = '\0';
}

/* pede o nome de um bairro e devolve o indice (-1 se nao existir) */
static int ler_bairro(Grafo *g, const char *prompt) {
    char nome[MAX_NOME];
    int idx;
    ler_palavra(prompt, nome);
    idx = grafo_indice(g, nome);
    if (idx == -1) printf("FALHA: o bairro '%s' nao existe.\n", nome);
    return idx;
}

static void op_caminho(Grafo *g) {
    int destino = ler_bairro(g, "Bairro de destino: ");
    if (destino != -1) analise_caminho_agua(g, destino, stdout);
}

static void op_rotura(Grafo *g) {
    int u = ler_bairro(g, "Tubo - bairro A: ");
    int v;
    if (u == -1) return;
    v = ler_bairro(g, "Tubo - bairro B: ");
    if (v == -1) return;
    analise_simular_rotura(g, u, v, stdout);
}

static void op_isolar(Grafo *g) {
    int area = ler_bairro(g, "Bairro com vazamento: ");
    if (area != -1) analise_isolar_vazamento(g, area, stdout);
}

static void op_adicionar_bairro(Grafo *g) {
    char nome[MAX_NOME];
    ler_palavra("Nome do novo bairro: ", nome);
    if (nome[0] == '\0') {
        printf("FALHA: nome invalido.\n");
        return;
    }
    if (grafo_adicionar_no(g, nome) == -1)
        printf("FALHA: bairro repetido ou rede cheia.\n");
    else
        printf("SUCESSO: bairro '%s' adicionado.\n", nome);
}

static void op_adicionar_tubo(Grafo *g) {
    int u = ler_bairro(g, "Bairro A: ");
    int v, comp;
    if (u == -1) return;
    v = ler_bairro(g, "Bairro B: ");
    if (v == -1) return;
    if (!ler_inteiro("Comprimento (m): ", &comp) || comp <= 0) {
        printf("FALHA: comprimento invalido.\n");
        return;
    }
    if (grafo_adicionar_tubo(g, u, v, comp))
        printf("SUCESSO: tubo %s -- %s criado.\n", grafo_nome(g, u), grafo_nome(g, v));
    else
        printf("FALHA: tubo repetido ou bairros invalidos.\n");
}

static void op_remover_tubo(Grafo *g) {
    int u = ler_bairro(g, "Bairro A: ");
    int v;
    if (u == -1) return;
    v = ler_bairro(g, "Bairro B: ");
    if (v == -1) return;
    if (grafo_remover_tubo(g, u, v))
        printf("SUCESSO: tubo removido.\n");
    else
        printf("FALHA: esse tubo nao existe.\n");
}

int main(void) {
    Grafo *g = grafo_criar(CAP_NOS);
    int op, carregada = 0;

    do {
        printf("\n===== SIMULADOR DA REDE DE AGUA =====\n");
        printf(" 1. Carregar cidade (ficheiro)\n");
        printf(" 2. Mostrar rede\n");
        printf(" 3. Caminho da agua: estacao -> bairro\n");
        printf(" 4. Simular rotura de uma tubulacao\n");
        printf(" 5. Nos criticos (pontos de articulacao)\n");
        printf(" 6. Tubulacoes criticas (pontes)\n");
        printf(" 7. Isolar vazamento (minimo de valvulas)\n");
        printf(" 8. Adicionar bairro\n");
        printf(" 9. Adicionar tubo\n");
        printf("10. Remover tubo\n");
        printf("11. Exportar relatorio para ficheiro\n");
        printf(" 0. Sair\n");
        if (!ler_inteiro("Opcao: ", &op)) {
            printf("FALHA: opcao invalida.\n");
            continue;
        }

        /* quase todas as opcoes precisam da cidade ja carregada */
        if (op != 1 && op != 0 && !carregada) {
            printf("FALHA: carregue primeiro a cidade (opcao 1).\n");
            continue;
        }

        switch (op) {
            case 1:
                if (carregada) printf("AVISO: a cidade ja foi carregada.\n");
                else if (ficheiro_ler_cidade(g, FICH_ENTRADA)) carregada = 1;
                break;
            case 2:  grafo_imprimir(g, stdout);                     break;
            case 3:  op_caminho(g);                                 break;
            case 4:  op_rotura(g);                                  break;
            case 5:  analise_nos_criticos(g, stdout);               break;
            case 6:  analise_tubos_criticos(g, stdout);             break;
            case 7:  op_isolar(g);                                  break;
            case 8:  op_adicionar_bairro(g);                        break;
            case 9:  op_adicionar_tubo(g);                          break;
            case 10: op_remover_tubo(g);                            break;
            case 11: ficheiro_escrever_relatorio(g, FICH_RELATORIO); break;
            case 0:  printf("A terminar o programa.\n");            break;
            default: printf("FALHA: opcao inexistente.\n");         break;
        }
    } while (op != 0);

    grafo_destruir(g);
    return 0;
}
