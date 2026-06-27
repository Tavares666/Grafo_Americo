#ifndef GRAFO_H
#define GRAFO_H

#define MAX_VERTICES 21
#define TAM_NOME 100
#define ETA_PRINCIPAL 0

/*
 * Um vertice representa um ponto importante da rede:
 * ETA, reservatorio, bomba, bairro ou estacao.
 */
typedef struct {
    char nome[TAM_NOME];
} Vertice;

typedef struct {
    Vertice vertices[MAX_VERTICES];
    int matrizPesos[MAX_VERTICES][MAX_VERTICES];
    int matrizAtiva[MAX_VERTICES][MAX_VERTICES];
    int totalVertices;
} Grafo;

void inicializarGrafo(Grafo *grafo);
void cadastrarRedeHydroMap(Grafo *grafo);
void adicionarVertice(Grafo *grafo, int indice, const char *nome);
void adicionarAresta(Grafo *grafo, int origem, int destino, int peso);
void listarVertices(const Grafo *grafo);
void listarTubulacoes(const Grafo *grafo);
void imprimirRedeCompleta(const Grafo *grafo);
int bloquearAresta(Grafo *grafo, int origem, int destino);
int reativarAresta(Grafo *grafo, int origem, int destino);
void liberarGrafo(Grafo *grafo);
const char *tipoVertice(const char *nome);

#endif
