#ifndef VISUALIZACAO_H
#define VISUALIZACAO_H

#include "grafo.h"

int gerarMapaHTML(const Grafo *grafo, const char *nomeArquivo);
int gerarMapaHTMLComCaminho(const Grafo *grafo, const char *nomeArquivo,
                            const int caminho[], int tamanhoCaminho);

#endif
