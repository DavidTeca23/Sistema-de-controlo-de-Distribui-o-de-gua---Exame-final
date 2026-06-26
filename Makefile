# ============================================================
#  Makefile - Simulador da rede de distribuicao de agua (EDII)
# ============================================================
CC     = gcc
CFLAGS = -Wall -Wextra -g -std=c99
SRC    = src/main.c src/grafo.c src/fila.c src/hash.c src/analise.c src/ficheiros.c
ALVO   = agua

all: $(ALVO)

$(ALVO): $(SRC)
	$(CC) $(CFLAGS) -o $(ALVO) $(SRC)

run: $(ALVO)
	./$(ALVO)

clean:
	rm -f $(ALVO)

.PHONY: all run clean
