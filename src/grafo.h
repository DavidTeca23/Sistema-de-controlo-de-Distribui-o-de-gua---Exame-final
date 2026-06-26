/* TAD Grafo - representa a rede de tubulacoes da cidade.
   Grafo nao dirigido: os nos sao bairros e as arestas sao tubos. */
#ifndef GRAFO_H
#define GRAFO_H

#include <stdio.h>
#include "hash.h"

typedef struct Tubo {
    int destino;
    int comprimento;     /* em metros */
    struct Tubo *prox;
} Tubo;

typedef struct {
    char nome[MAX_NOME];
    Tubo *tubos;         /* lista de adjacencia */
} No;

typedef struct {
    No  *nos;
    int n;               /* numero de bairros */
    int capacidade;
    int fonte;           /* indice da estacao de tratamento */
    TabelaHash *indice;  /* nome -> indice */
} Grafo;

Grafo *grafo_criar(int capacidade);
void   grafo_destruir(Grafo *g);

int  grafo_adicionar_no(Grafo *g, const char *nome);
int  grafo_adicionar_tubo(Grafo *g, int u, int v, int comp);
int  grafo_remover_tubo(Grafo *g, int u, int v);
void grafo_definir_fonte(Grafo *g, int idx);

int  grafo_indice(Grafo *g, const char *nome);
const char *grafo_nome(Grafo *g, int idx);
int  grafo_existe_tubo(Grafo *g, int u, int v);

void grafo_imprimir(Grafo *g, FILE *saida);

#endif
