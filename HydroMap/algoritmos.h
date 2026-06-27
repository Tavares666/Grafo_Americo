#ifndef ALGORITMOS_H
#define ALGORITMOS_H

#include "grafo.h"

void dijkstra(const Grafo *grafo, int origem, int destino);
int dijkstraComCaminho(const Grafo *grafo, int origem, int destino,
                       int caminho[], int *tamanhoCaminho, int *custoTotal);
void bfs(const Grafo *grafo, int origem, int visitado[]);
void executarBFS(const Grafo *grafo, int origem);
void executarDFS(const Grafo *grafo, int origem);
void listarDesabastecidos(const Grafo *grafo, const int visitado[]);

#endif
