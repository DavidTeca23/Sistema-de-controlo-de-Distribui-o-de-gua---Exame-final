#include <stdio.h>
#include <stdlib.h>
#include "fila.h"

Fila *fila_criar(void) {
    Fila *f = malloc(sizeof(Fila));
    f->frente = NULL;
    f->fim = NULL;
    f->tamanho = 0;
    return f;
}

void fila_destruir(Fila *f) {
    if (f == NULL) return;
    while (!fila_vazia(f)) fila_remover(f);
    free(f);
}

int fila_vazia(Fila *f) {
    return f->tamanho == 0;
}

void fila_inserir(Fila *f, int valor) {
    NoFila *no = malloc(sizeof(NoFila));
    no->valor = valor;
    no->prox = NULL;

    if (f->fim == NULL) {
        f->frente = no;
        f->fim = no;
    } else {
        f->fim->prox = no;
        f->fim = no;
    }
    f->tamanho++;
}

int fila_remover(Fila *f) {
    NoFila *no;
    int v;

    if (fila_vazia(f)) return -1;

    no = f->frente;
    v = no->valor;
    f->frente = no->prox;
    if (f->frente == NULL) f->fim = NULL;   /* esvaziou */
    free(no);
    f->tamanho--;
    return v;
}
