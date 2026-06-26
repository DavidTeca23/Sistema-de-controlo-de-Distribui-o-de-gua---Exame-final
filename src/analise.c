#include <stdio.h>
#include <stdlib.h>
#include "analise.h"
#include "fila.h"

/* Caminho da agua da estacao ate um bairro, usando BFS. */
void analise_caminho_agua(Grafo *g, int destino, FILE *saida) {
    int n = g->n, fonte = g->fonte, atual, i, tam;
    int *anterior = malloc(n * sizeof(int));
    int *visitado = calloc(n, sizeof(int));
    int *caminho  = malloc(n * sizeof(int));
    Fila *f = fila_criar();

    fprintf(saida, "\n--- Caminho da agua: %s -> %s ---\n",
            grafo_nome(g, fonte), grafo_nome(g, destino));

    for (i = 0; i < n; i++) anterior[i] = -1;
    visitado[fonte] = 1;
    fila_inserir(f, fonte);

    while (!fila_vazia(f)) {
        Tubo *t;
        atual = fila_remover(f);
        if (atual == destino) break;
        for (t = g->nos[atual].tubos; t != NULL; t = t->prox) {
            if (!visitado[t->destino]) {
                visitado[t->destino] = 1;
                anterior[t->destino] = atual;
                fila_inserir(f, t->destino);
            }
        }
    }

    if (!visitado[destino]) {
        fprintf(saida, "Nao ha caminho: %s nao recebe agua da estacao.\n",
                grafo_nome(g, destino));
    } else {
        /* refaz o caminho do destino ate a fonte e depois imprime ao contrario */
        tam = 0;
        for (i = destino; i != -1; i = anterior[i]) caminho[tam++] = i;
        fprintf(saida, "Percurso: ");
        for (i = tam - 1; i >= 0; i--)
            fprintf(saida, "%s%s", grafo_nome(g, caminho[i]), i ? " -> " : "\n");
        fprintf(saida, "Distancia: %d salto(s) de tubulacao.\n", tam - 1);
    }

    fila_destruir(f);
    free(anterior);
    free(visitado);
    free(caminho);
}

/* BFS a partir da fonte que marca os bairros alcancaveis.
   Pode ignorar um tubo (serve para simular uma rotura). */
static void alcancaveis(Grafo *g, int *visitado, int ign_u, int ign_v) {
    int i, atual;
    Fila *f = fila_criar();

    for (i = 0; i < g->n; i++) visitado[i] = 0;
    visitado[g->fonte] = 1;
    fila_inserir(f, g->fonte);

    while (!fila_vazia(f)) {
        Tubo *t;
        atual = fila_remover(f);
        for (t = g->nos[atual].tubos; t != NULL; t = t->prox) {
            if ((atual == ign_u && t->destino == ign_v) ||
                (atual == ign_v && t->destino == ign_u)) continue;
            if (!visitado[t->destino]) {
                visitado[t->destino] = 1;
                fila_inserir(f, t->destino);
            }
        }
    }
    fila_destruir(f);
}

/* Simula o rompimento de um tubo e mostra que bairros ficam sem agua. */
void analise_simular_rotura(Grafo *g, int u, int v, FILE *saida) {
    int *visitado = malloc(g->n * sizeof(int));
    int i, sem_agua = 0;

    fprintf(saida, "\n--- Rotura do tubo %s -- %s ---\n",
            grafo_nome(g, u), grafo_nome(g, v));
    if (!grafo_existe_tubo(g, u, v)) {
        fprintf(saida, "FALHA: esse tubo nao existe.\n");
        free(visitado);
        return;
    }

    alcancaveis(g, visitado, u, v);

    fprintf(saida, "Bairros sem agua: ");
    for (i = 0; i < g->n; i++) {
        if (!visitado[i]) {
            fprintf(saida, "%s ", grafo_nome(g, i));
            sem_agua++;
        }
    }
    if (sem_agua == 0) fprintf(saida, "nenhum (a rede continua toda abastecida).");
    fprintf(saida, "\nTotal sem agua: %d bairro(s).\n", sem_agua);

    free(visitado);
}

/* ---- Pontos de articulacao (nos criticos) ----
   DFS guardando o tempo de descoberta (disc) e o menor tempo alcancavel
   (low). Um no e critico se algum filho na DFS nao consegue voltar para
   cima dele por outro caminho. */
static int *disc, *low, *vis, *art, tempo_dfs;

static void dfs_art(Grafo *g, int u, int pai) {
    int filhos = 0;
    Tubo *t;

    vis[u] = 1;
    disc[u] = low[u] = ++tempo_dfs;

    for (t = g->nos[u].tubos; t != NULL; t = t->prox) {
        int w = t->destino;
        if (!vis[w]) {
            filhos++;
            dfs_art(g, w, u);
            if (low[w] < low[u]) low[u] = low[w];
            if (pai != -1 && low[w] >= disc[u]) art[u] = 1;
        } else if (w != pai) {
            if (disc[w] < low[u]) low[u] = disc[w];
        }
    }
    /* a raiz so e critica se tiver mais que um filho na DFS */
    if (pai == -1 && filhos > 1) art[u] = 1;
}

void analise_nos_criticos(Grafo *g, FILE *saida) {
    int n = g->n, i, total = 0;

    disc = calloc(n, sizeof(int));
    low  = calloc(n, sizeof(int));
    vis  = calloc(n, sizeof(int));
    art  = calloc(n, sizeof(int));
    tempo_dfs = 0;

    for (i = 0; i < n; i++)
        if (!vis[i]) dfs_art(g, i, -1);

    fprintf(saida, "\n--- Nos criticos (pontos de articulacao) ---\n");
    fprintf(saida, "Bairros que, ao falhar, dividem a rede: ");
    for (i = 0; i < n; i++) {
        if (art[i]) {
            fprintf(saida, "%s ", grafo_nome(g, i));
            total++;
        }
    }
    if (total == 0) fprintf(saida, "nenhum (a rede e robusta).");
    fprintf(saida, "\nTotal: %d no(s) critico(s).\n", total);

    free(disc); free(low); free(vis); free(art);
}

/* ---- Pontes (tubulacoes criticas) ----
   Mesma ideia da DFS anterior. Um tubo (u,w) e ponte quando o filho w nao
   tem nenhuma aresta de retorno que chegue a u ou acima dele: low[w] > disc[u]. */
static FILE *saida_ponte;
static Grafo *g_ponte;
static int total_pontes;

static void dfs_ponte(int u, int pai) {
    Tubo *t;
    vis[u] = 1;
    disc[u] = low[u] = ++tempo_dfs;

    for (t = g_ponte->nos[u].tubos; t != NULL; t = t->prox) {
        int w = t->destino;
        if (!vis[w]) {
            dfs_ponte(w, u);
            if (low[w] < low[u]) low[u] = low[w];
            if (low[w] > disc[u]) {
                fprintf(saida_ponte, "%s -- %s   ",
                        grafo_nome(g_ponte, u), grafo_nome(g_ponte, w));
                total_pontes++;
            }
        } else if (w != pai) {
            if (disc[w] < low[u]) low[u] = disc[w];
        }
    }
}

void analise_tubos_criticos(Grafo *g, FILE *saida) {
    int n = g->n, i;

    disc = calloc(n, sizeof(int));
    low  = calloc(n, sizeof(int));
    vis  = calloc(n, sizeof(int));
    tempo_dfs = 0;
    saida_ponte = saida;
    g_ponte = g;
    total_pontes = 0;

    fprintf(saida, "\n--- Tubulacoes criticas (pontes) ---\n");
    fprintf(saida, "Tubos cuja rotura isola bairros: ");
    for (i = 0; i < n; i++)
        if (!vis[i]) dfs_ponte(i, -1);
    if (total_pontes == 0) fprintf(saida, "nenhuma (toda tubulacao tem alternativa).");
    fprintf(saida, "\nTotal: %d tubo(s) critico(s).\n", total_pontes);

    free(disc); free(low); free(vis);
}

/* ---- Isolar um vazamento com o minimo de valvulas ----
   Cada tubo vale 1 (uma valvula). O numero minimo de valvulas a fechar para
   cortar a area da fonte e o fluxo maximo entre as duas (Edmonds-Karp): vamos
   procurando caminhos da fonte ate a area e somando 1 por cada um. */

static int bfs_residual(int **cap, int n, int s, int t, int *pai) {
    int i, atual;
    int *visitado = calloc(n, sizeof(int));
    Fila *f = fila_criar();

    for (i = 0; i < n; i++) pai[i] = -1;
    visitado[s] = 1;
    fila_inserir(f, s);

    while (!fila_vazia(f)) {
        atual = fila_remover(f);
        for (i = 0; i < n; i++) {
            if (!visitado[i] && cap[atual][i] > 0) {
                visitado[i] = 1;
                pai[i] = atual;
                if (i == t) {
                    fila_destruir(f);
                    free(visitado);
                    return 1;
                }
                fila_inserir(f, i);
            }
        }
    }
    fila_destruir(f);
    free(visitado);
    return 0;
}

void analise_isolar_vazamento(Grafo *g, int area, FILE *saida) {
    int n = g->n, s = g->fonte, i, j, fluxo = 0;
    int **cap, *pai, *lado;

    fprintf(saida, "\n--- Isolar vazamento em %s ---\n", grafo_nome(g, area));
    if (area == s) {
        fprintf(saida, "FALHA: o vazamento esta na propria estacao de tratamento.\n");
        return;
    }

    /* matriz de capacidades: 1 por tubo, nos dois sentidos */
    cap = malloc(n * sizeof(int *));
    for (i = 0; i < n; i++) cap[i] = calloc(n, sizeof(int));
    for (i = 0; i < n; i++) {
        Tubo *t;
        for (t = g->nos[i].tubos; t != NULL; t = t->prox)
            cap[i][t->destino] = 1;
    }

    pai = malloc(n * sizeof(int));
    while (bfs_residual(cap, n, s, area, pai)) {
        for (i = area; i != s; i = pai[i]) {
            cap[pai[i]][i] -= 1;
            cap[i][pai[i]] += 1;
        }
        fluxo++;
    }

    /* o corte fica do lado do vazamento: bairros que ainda conseguem chegar
       a area no grafo residual (percorrendo os tubos ao contrario) */
    lado = calloc(n, sizeof(int));
    {
        int atual;
        Fila *f = fila_criar();
        lado[area] = 1;
        fila_inserir(f, area);
        while (!fila_vazia(f)) {
            atual = fila_remover(f);
            for (i = 0; i < n; i++)
                if (!lado[i] && cap[i][atual] > 0) {
                    lado[i] = 1;
                    fila_inserir(f, i);
                }
        }
        fila_destruir(f);
    }

    if (fluxo == 0) {
        fprintf(saida, "%s ja esta isolado da estacao (nenhuma valvula a fechar).\n",
                grafo_nome(g, area));
    } else {
        fprintf(saida, "Valvulas a fechar (minimo): %d\n", fluxo);
        for (i = 0; i < n; i++) {
            Tubo *t;
            for (t = g->nos[i].tubos; t != NULL; t = t->prox) {
                j = t->destino;
                if (i < j && lado[i] != lado[j])
                    fprintf(saida, "   fechar valvula: %s -- %s\n",
                            grafo_nome(g, i), grafo_nome(g, j));
            }
        }
        fprintf(saida, "Area isolada: ");
        for (i = 0; i < n; i++) if (lado[i]) fprintf(saida, "%s ", grafo_nome(g, i));
        fprintf(saida, "\nO resto da cidade continua ligado a estacao.\n");
    }

    for (i = 0; i < n; i++) free(cap[i]);
    free(cap);
    free(pai);
    free(lado);
}
