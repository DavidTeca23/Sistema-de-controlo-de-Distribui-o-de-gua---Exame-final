/* Leitura da cidade a partir de um ficheiro e escrita do relatorio. */
#ifndef FICHEIROS_H
#define FICHEIROS_H

#include "grafo.h"

int ficheiro_ler_cidade(Grafo *g, const char *nome);
int ficheiro_escrever_relatorio(Grafo *g, const char *nome);

#endif
