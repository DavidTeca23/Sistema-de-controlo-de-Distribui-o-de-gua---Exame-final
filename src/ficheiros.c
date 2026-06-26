#include <stdio.h>
#include "ficheiros.h"
#include "analise.h"

/* Formato do ficheiro:
     N M
     <estacao>            (1o nome = fonte)
     <bairro> ...         (N nomes no total)
     <u> <v> <comprimento>  (M tubos, por nome)
*/
int ficheiro_ler_cidade(Grafo *g, const char *nome) {
    FILE *f = fopen(nome, "r");
    int N, M, i, comp;
    char nomeU[MAX_NOME], nomeV[MAX_NOME];

    if (f == NULL) {
        printf("FALHA: nao foi possivel abrir '%s'.\n", nome);
        return 0;
    }
    if (fscanf(f, "%d %d", &N, &M) != 2) {
        printf("FALHA: cabecalho invalido (esperado: N M).\n");
        fclose(f);
        return 0;
    }

    for (i = 0; i < N; i++) {
        if (fscanf(f, "%47s", nomeU) != 1) {
            printf("FALHA: faltam nomes de bairros.\n");
            fclose(f);
            return 0;
        }
        grafo_adicionar_no(g, nomeU);
    }
    grafo_definir_fonte(g, 0);

    for (i = 0; i < M; i++) {
        int u, v;
        if (fscanf(f, "%47s %47s %d", nomeU, nomeV, &comp) != 3) {
            printf("FALHA: tubo %d com formato invalido.\n", i + 1);
            fclose(f);
            return 0;
        }
        u = grafo_indice(g, nomeU);
        v = grafo_indice(g, nomeV);
        if (u == -1 || v == -1) {
            printf("FALHA: tubo %d refere bairro inexistente (%s ou %s).\n",
                   i + 1, nomeU, nomeV);
            fclose(f);
            return 0;
        }
        grafo_adicionar_tubo(g, u, v, comp);
    }
    fclose(f);
    printf("SUCESSO: cidade carregada (%d bairros, %d tubos). Fonte: %s.\n",
           N, M, grafo_nome(g, g->fonte));
    return 1;
}

int ficheiro_escrever_relatorio(Grafo *g, const char *nome) {
    FILE *f = fopen(nome, "w");
    if (f == NULL) {
        printf("FALHA: nao foi possivel escrever em '%s'.\n", nome);
        return 0;
    }
    fprintf(f, "===== RELATORIO DA REDE DE AGUA =====\n");
    grafo_imprimir(g, f);
    analise_nos_criticos(g, f);
    analise_tubos_criticos(g, f);
    fclose(f);
    printf("SUCESSO: relatorio exportado para '%s'.\n", nome);
    return 1;
}
