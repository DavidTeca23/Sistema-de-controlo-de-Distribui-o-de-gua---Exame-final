/* TAD Tabela Hash: guarda o nome de cada bairro e o seu indice no grafo.
   Assim podemos procurar um bairro pelo nome sem percorrer o grafo todo. */
#ifndef HASH_H
#define HASH_H

#define TAM_HASH  211
#define MAX_NOME  48

typedef struct NoHash {
    char chave[MAX_NOME];   /* nome do bairro */
    int  valor;             /* indice no grafo */
    struct NoHash *prox;
} NoHash;

typedef struct {
    NoHash *baldes[TAM_HASH];
    int total;
} TabelaHash;

TabelaHash *hash_criar(void);
void        hash_destruir(TabelaHash *t);
int hash_inserir(TabelaHash *t, const char *chave, int valor);
int hash_obter(TabelaHash *t, const char *chave, int *valor);

#endif
