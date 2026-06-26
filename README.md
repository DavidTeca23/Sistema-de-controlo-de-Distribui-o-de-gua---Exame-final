# Simulador da Rede de Distribuição de Água

Trabalho final de **Estrutura de Dados II** — Engenharia Informática, ISPTEC.

Programa em linguagem C que modela a rede de tubulações de uma cidade como um
grafo e ajuda a resolver problemas de **vazamento e isolamento de sectores**:
fechar o menor número de válvulas para isolar uma fuga, descobrir o caminho da
água da estação de tratamento até um bairro, simular a rotura de uma tubulação
e identificar os pontos críticos da rede.

## O problema

Numa cidade, a água parte de uma estação de tratamento e percorre uma rede de
tubos até aos vários bairros. Cada tubo tem uma válvula. Quando há um vazamento,
é preciso isolar a zona afectada fechando o menor número possível de válvulas,
sem deixar o resto da cidade sem água. Também interessa saber por onde a água
chega a cada bairro, prever o impacto da rotura de um tubo e descobrir que
bairros ou tubos são tão importantes que, ao falharem, dividem a rede.

## Funcionalidades

- Carregar a rede de uma cidade a partir de um ficheiro
- Mostrar a rede (bairros e tubos)
- Encontrar o caminho da água: estação → bairro
- Simular a rotura de um tubo e listar os bairros que ficam sem água
- Identificar os **nós críticos** (bairros cuja falha divide a rede)
- Identificar as **tubulações críticas** (tubos cuja rotura isola bairros)
- Isolar um vazamento fechando o **mínimo de válvulas**
- Adicionar/remover bairros e tubos
- Exportar um relatório para ficheiro

## Estruturas de dados (TAD)

O trabalho usa três Tipos Abstractos de Dados, cada um no seu par `.h`/`.c`:

| TAD | Ficheiros | Para que serve |
|-----|-----------|----------------|
| **Grafo** (lista de adjacências) | `grafo.h` / `grafo.c` | Guarda a rede: bairros e tubos |
| **Fila** (FIFO) | `fila.h` / `fila.c` | Apoia as pesquisas em largura (BFS) |
| **Tabela Hash** | `hash.h` / `hash.c` | Procura um bairro pelo nome (nome → índice) |

Os algoritmos de análise estão em `analise.h` / `analise.c`, a leitura e escrita
em ficheiro em `ficheiros.h` / `ficheiros.c`, e o **menu principal** em `main.c`.

## Algoritmos usados

| Funcionalidade | Algoritmo |
|----------------|-----------|
| Caminho da água | Pesquisa em largura (BFS) |
| Rotura → bairros sem água | Remoção do tubo + BFS de alcance |
| Nós críticos | Pontos de articulação (Tarjan / DFS) |
| Tubulações críticas | Pontes (Tarjan / DFS) |
| Isolar vazamento | Corte mínimo (fluxo máximo, Edmonds-Karp) |

## Como compilar e executar

Na pasta do projecto, com o `make`:

```
make
make run
```

Ou directamente com o gcc:

```
gcc -Wall -Wextra -std=c99 -o agua src/*.c
./agua
```

No Windows (Code::Blocks ou MinGW), basta incluir todos os ficheiros `.c` da
pasta `src` no projecto e compilar.

## Ficheiro de entrada

A rede é lida de `dados/cidade.txt`. O formato é:

```
N M
<estacao_de_tratamento>     (1.º nome é a fonte de água)
<bairro_2> ... <bairro_N>
<bairroU> <bairroV> <comprimento>    (M linhas de tubos)
```

Os nomes dos bairros não podem ter espaços (usar `_` se for preciso). Exemplo:

```
8 10
Estacao
Maianga
Talatona
Kilamba
Cazenga
Viana
Rangel
Cacuaco
Estacao Maianga 800
Estacao Talatona 1200
Maianga Talatona 500
Maianga Kilamba 600
Talatona Kilamba 700
Kilamba Cazenga 900
Cazenga Viana 1000
Cazenga Rangel 650
Rangel Cacuaco 750
Cacuaco Cazenga 1100
```

## Organização dos ficheiros

```
ExameEDII_Turma_NGrupo/
├── src/
│   ├── grafo.h / grafo.c          TAD Grafo
│   ├── fila.h / fila.c            TAD Fila
│   ├── hash.h / hash.c            TAD Tabela Hash
│   ├── analise.h / analise.c      Algoritmos de análise
│   ├── ficheiros.h / ficheiros.c  Leitura e escrita em ficheiro
│   └── main.c                     Menu principal
├── dados/
│   └── cidade.txt                 Rede de entrada
├── Makefile
└── README.md
```

## Equipa e responsabilidades

| Membro | Responsabilidade |
|--------|------------------|
| David Miguel Teca Nzazi | TAD Grafo e Tabela Hash; integração |
| Kelvin Cucubica- 20220056  | TAD Fila e BFS (caminho da água, rotura) |
| Kelvin Cucubica- 20220056  | Nós críticos e pontes |
| Garcia Neto Cassange - 20242990 | Isolamento de vazamentos e ficheiros |
| David Miguel Teca Nzazi| Menu, testes e relatório |

---
Docente: Sílvia António — ISPTEC, 2026
