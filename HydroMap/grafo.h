#ifndef GRAFO_H
#define GRAFO_H

#define MAX_VERTICES 21
#define TAM_NOME 100
#define ETA_PRINCIPAL 0

/*
 * Uma aresta representa uma tubulacao/adutora.
 * Como a rede tem sentido de fluxo, a aresta sai de uma origem
 * e aponta para um destino. O peso representa distancia ou perda de carga.
 */
typedef struct Aresta {
    int destino;
    int peso;
    int ativa;
    struct Aresta *proxima;
} Aresta;

/*
 * Um vertice representa um ponto importante da rede:
 * ETA, reservatorio, bomba, bairro ou estacao.
 * Cada vertice guarda uma lista de arestas que saem dele.
 */
typedef struct {
    char nome[TAM_NOME];
    Aresta *lista;
} Vertice;

typedef struct {
    Vertice vertices[MAX_VERTICES];
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
