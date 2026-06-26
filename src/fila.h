/* TAD Fila (FIFO) de inteiros, usada nas pesquisas em largura. */
#ifndef FILA_H
#define FILA_H

typedef struct NoFila {
    int valor;
    struct NoFila *prox;
} NoFila;

typedef struct {
    NoFila *frente;
    NoFila *fim;
    int tamanho;
} Fila;

Fila *fila_criar(void);
void  fila_destruir(Fila *f);
int   fila_vazia(Fila *f);
void  fila_inserir(Fila *f, int valor);
int   fila_remover(Fila *f);   /* devolve -1 se estiver vazia */

#endif
