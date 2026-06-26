#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "grafo.h"

/* coloca um tubo u->v na lista (so um sentido) */
static void inserir_tubo(No *no, int destino, int comp) {
    Tubo *t = malloc(sizeof(Tubo));
    t->destino = destino;
    t->comprimento = comp;
    t->prox = no->tubos;
    no->tubos = t;
}

static int remover_tubo_un(No *no, int destino) {
    Tubo *ant = NULL, *t = no->tubos;
    while (t != NULL) {
        if (t->destino == destino) {
            if (ant == NULL) no->tubos = t->prox;
            else ant->prox = t->prox;
            free(t);
            return 1;
        }
        ant = t;
        t = t->prox;
    }
    return 0;
}

Grafo *grafo_criar(int capacidade) {
    Grafo *g = malloc(sizeof(Grafo));
    g->nos = malloc(capacidade * sizeof(No));
    g->n = 0;
    g->capacidade = capacidade;
    g->fonte = 0;
    g->indice = hash_criar();
    return g;
}

void grafo_destruir(Grafo *g) {
    int i;
    if (g == NULL) return;
    for (i = 0; i < g->n; i++) {
        Tubo *t = g->nos[i].tubos;
        while (t != NULL) {
            Tubo *p = t->prox;
            free(t);
            t = p;
        }
    }
    free(g->nos);
    hash_destruir(g->indice);
    free(g);
}

int grafo_adicionar_no(Grafo *g, const char *nome) {
    int idx;
    if (grafo_indice(g, nome) != -1) return -1;   /* ja existe */
    if (g->n >= g->capacidade) return -1;

    idx = g->n;
    strncpy(g->nos[idx].nome, nome, MAX_NOME - 1);
    g->nos[idx].nome[MAX_NOME - 1] = '\0';
    g->nos[idx].tubos = NULL;
    hash_inserir(g->indice, nome, idx);
    g->n++;
    return idx;
}

/* tubo nao dirigido: tem de ficar nas duas listas */
int grafo_adicionar_tubo(Grafo *g, int u, int v, int comp) {
    if (u < 0 || v < 0 || u >= g->n || v >= g->n) return 0;
    if (u == v || comp <= 0) return 0;
    if (grafo_existe_tubo(g, u, v)) return 0;

    inserir_tubo(&g->nos[u], v, comp);
    inserir_tubo(&g->nos[v], u, comp);
    return 1;
}

int grafo_remover_tubo(Grafo *g, int u, int v) {
    if (u < 0 || v < 0 || u >= g->n || v >= g->n) return 0;
    if (remover_tubo_un(&g->nos[u], v) && remover_tubo_un(&g->nos[v], u))
        return 1;
    return 0;
}

void grafo_definir_fonte(Grafo *g, int idx) {
    if (idx >= 0 && idx < g->n) g->fonte = idx;
}

int grafo_indice(Grafo *g, const char *nome) {
    int idx;
    if (hash_obter(g->indice, nome, &idx)) return idx;
    return -1;
}

const char *grafo_nome(Grafo *g, int idx) {
    if (idx < 0 || idx >= g->n) return "?";
    return g->nos[idx].nome;
}

int grafo_existe_tubo(Grafo *g, int u, int v) {
    Tubo *t;
    if (u < 0 || u >= g->n) return 0;
    for (t = g->nos[u].tubos; t != NULL; t = t->prox)
        if (t->destino == v) return 1;
    return 0;
}

void grafo_imprimir(Grafo *g, FILE *saida) {
    int i;
    fprintf(saida, "\n--- Rede da cidade (%d bairros) ---\n", g->n);
    fprintf(saida, "Estacao de tratamento (fonte): %s\n", grafo_nome(g, g->fonte));
    for (i = 0; i < g->n; i++) {
        Tubo *t;
        fprintf(saida, "%-14s :", g->nos[i].nome);
        if (g->nos[i].tubos == NULL) fprintf(saida, " (sem tubos)");
        for (t = g->nos[i].tubos; t != NULL; t = t->prox)
            fprintf(saida, " %s(%dm)", grafo_nome(g, t->destino), t->comprimento);
        fprintf(saida, "\n");
    }
}
