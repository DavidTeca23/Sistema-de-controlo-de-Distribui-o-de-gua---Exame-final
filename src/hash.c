#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash.h"

/* funcao de hash djb2 */
static unsigned long dispersao(const char *s) {
    unsigned long h = 5381;
    int c;
    while ((c = (unsigned char) *s++))
        h = h * 33 + c;
    return h % TAM_HASH;
}

static NoHash *procurar(TabelaHash *t, const char *chave) {
    NoHash *no = t->baldes[dispersao(chave)];
    while (no != NULL) {
        if (strcmp(no->chave, chave) == 0) return no;
        no = no->prox;
    }
    return NULL;
}

TabelaHash *hash_criar(void) {
    int i;
    TabelaHash *t = malloc(sizeof(TabelaHash));
    for (i = 0; i < TAM_HASH; i++) t->baldes[i] = NULL;
    t->total = 0;
    return t;
}

void hash_destruir(TabelaHash *t) {
    int i;
    if (t == NULL) return;
    for (i = 0; i < TAM_HASH; i++) {
        NoHash *no = t->baldes[i];
        while (no != NULL) {
            NoHash *p = no->prox;
            free(no);
            no = p;
        }
    }
    free(t);
}

int hash_inserir(TabelaHash *t, const char *chave, int valor) {
    unsigned long i;
    NoHash *novo;

    if (procurar(t, chave) != NULL) return 0;   /* chave repetida */

    i = dispersao(chave);
    novo = malloc(sizeof(NoHash));
    strncpy(novo->chave, chave, MAX_NOME - 1);
    novo->chave[MAX_NOME - 1] = '\0';
    novo->valor = valor;
    novo->prox = t->baldes[i];
    t->baldes[i] = novo;
    t->total++;
    return 1;
}

int hash_obter(TabelaHash *t, const char *chave, int *valor) {
    NoHash *no = procurar(t, chave);
    if (no == NULL) return 0;
    *valor = no->valor;
    return 1;
}
