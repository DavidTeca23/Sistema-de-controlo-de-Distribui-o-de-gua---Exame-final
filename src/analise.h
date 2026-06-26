/* Algoritmos de analise da rede: caminho da agua, simulacao de rotura,
   nos criticos (pontos de articulacao), pontes e isolamento de vazamentos. */
#ifndef ANALISE_H
#define ANALISE_H

#include <stdio.h>
#include "grafo.h"

void analise_caminho_agua(Grafo *g, int destino, FILE *saida);
void analise_simular_rotura(Grafo *g, int u, int v, FILE *saida);
void analise_nos_criticos(Grafo *g, FILE *saida);
void analise_tubos_criticos(Grafo *g, FILE *saida);
void analise_isolar_vazamento(Grafo *g, int area, FILE *saida);

#endif
