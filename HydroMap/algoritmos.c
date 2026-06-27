#include <limits.h>
#include <stdio.h>

#include "algoritmos.h"

#define INFINITO 999999

static int menorDistancia(const int dist[], const int processado[], int total) {
    int i;
    int menor = INFINITO;
    int indice = -1;

    for (i = 0; i < total; i++) {
        if (!processado[i] && dist[i] < menor) {
            menor = dist[i];
            indice = i;
        }
    }

    return indice;
}

static void imprimirCaminho(const Grafo *grafo, const int anterior[], int destino) {
    int pilha[MAX_VERTICES];
    int tamanho = 0;
    int atual = destino;
    int i;

    while (atual != -1 && atual != -2 && tamanho < MAX_VERTICES) {
        pilha[tamanho++] = atual;
        atual = anterior[atual];
    }

    for (i = tamanho - 1; i >= 0; i--) {
        printf("%s", grafo->vertices[pilha[i]].nome);
        if (i > 0) {
            printf(" -> ");
        }
    }
}

void dijkstra(const Grafo *grafo, int origem, int destino) {
    int caminho[MAX_VERTICES];
    int tamanhoCaminho;
    int custoTotal;

    if (dijkstraComCaminho(grafo, origem, destino, caminho,
                           &tamanhoCaminho, &custoTotal)) {
        (void) caminho;
        (void) tamanhoCaminho;
        (void) custoTotal;
    }
}

int dijkstraComCaminho(const Grafo *grafo, int origem, int destino,
                       int caminho[], int *tamanhoCaminho, int *custoTotal) {
    int dist[MAX_VERTICES];
    int anterior[MAX_VERTICES];
    int processado[MAX_VERTICES];
    int i;
    int contador;
    int atual;
    int pilha[MAX_VERTICES];
    int tamanhoPilha = 0;
    Aresta *aresta;

    *tamanhoCaminho = 0;
    *custoTotal = 0;

    if (origem < 0 || origem >= grafo->totalVertices ||
        destino < 0 || destino >= grafo->totalVertices) {
        printf("Origem ou destino invalido.\n");
        return 0;
    }

    /*
     * Dijkstra calcula o menor caminho em grafo ponderado sem pesos negativos.
     * A cada rodada, escolhe o vertice ainda nao processado com menor custo
     * conhecido e tenta melhorar o custo dos seus vizinhos.
     */
    for (i = 0; i < grafo->totalVertices; i++) {
        dist[i] = INFINITO;
        anterior[i] = -1;
        processado[i] = 0;
    }
    dist[origem] = 0;
    anterior[origem] = -2;

    for (contador = 0; contador < grafo->totalVertices - 1; contador++) {
        atual = menorDistancia(dist, processado, grafo->totalVertices);
        if (atual == -1) {
            break;
        }

        processado[atual] = 1;
        aresta = grafo->vertices[atual].lista;
        while (aresta != NULL) {
            if (aresta->ativa &&
                !processado[aresta->destino] &&
                dist[atual] + aresta->peso < dist[aresta->destino]) {
                dist[aresta->destino] = dist[atual] + aresta->peso;
                anterior[aresta->destino] = atual;
            }
            aresta = aresta->proxima;
        }
    }

    printf("\n--- Caminho mais eficiente ---\n");
    printf("Origem : %s\n", grafo->vertices[origem].nome);
    printf("Destino: %s\n", grafo->vertices[destino].nome);

    if (dist[destino] == INFINITO) {
        printf("Nao existe caminho ativo entre os vertices escolhidos.\n");
        return 0;
    }

    atual = destino;
    while (atual != -1 && atual != -2 && tamanhoPilha < MAX_VERTICES) {
        pilha[tamanhoPilha++] = atual;
        atual = anterior[atual];
    }

    for (i = tamanhoPilha - 1; i >= 0; i--) {
        caminho[*tamanhoCaminho] = pilha[i];
        (*tamanhoCaminho)++;
    }
    *custoTotal = dist[destino];

    printf("Custo total: %d\n", dist[destino]);
    printf("Caminho: ");
    imprimirCaminho(grafo, anterior, destino);
    printf("\n");
    return 1;
}

void bfs(const Grafo *grafo, int origem, int visitado[]) {
    int fila[MAX_VERTICES];
    int inicio = 0;
    int fim = 0;
    int i;
    int atual;
    Aresta *aresta;

    for (i = 0; i < grafo->totalVertices; i++) {
        visitado[i] = 0;
    }

    if (origem < 0 || origem >= grafo->totalVertices) {
        return;
    }

    /*
     * BFS percorre em largura: visita primeiro os vizinhos mais proximos.
     * Na manutencao, todo vertice nao visitado pela BFS a partir da ETA
     * ficou sem caminho ativo de abastecimento.
     */
    visitado[origem] = 1;
    fila[fim++] = origem;

    while (inicio < fim) {
        atual = fila[inicio++];
        aresta = grafo->vertices[atual].lista;

        while (aresta != NULL) {
            if (aresta->ativa && !visitado[aresta->destino]) {
                visitado[aresta->destino] = 1;
                fila[fim++] = aresta->destino;
            }
            aresta = aresta->proxima;
        }
    }
}

void executarBFS(const Grafo *grafo, int origem) {
    int visitado[MAX_VERTICES];
    int i;

    bfs(grafo, origem, visitado);

    printf("\n--- BFS a partir de %s ---\n", grafo->vertices[origem].nome);
    printf("Vertices alcancaveis:\n");
    for (i = 0; i < grafo->totalVertices; i++) {
        if (visitado[i]) {
            printf("%2d - %s\n", i, grafo->vertices[i].nome);
        }
    }
}

static void dfsRecursivo(const Grafo *grafo, int atual, int visitado[]) {
    Aresta *aresta;

    visitado[atual] = 1;
    printf("%2d - %s\n", atual, grafo->vertices[atual].nome);

    /*
     * DFS percorre em profundidade: segue um caminho ate onde for possivel
     * antes de voltar e tentar outra ramificacao da rede.
     */
    aresta = grafo->vertices[atual].lista;
    while (aresta != NULL) {
        if (aresta->ativa && !visitado[aresta->destino]) {
            dfsRecursivo(grafo, aresta->destino, visitado);
        }
        aresta = aresta->proxima;
    }
}

void executarDFS(const Grafo *grafo, int origem) {
    int visitado[MAX_VERTICES];
    int i;

    for (i = 0; i < grafo->totalVertices; i++) {
        visitado[i] = 0;
    }

    if (origem < 0 || origem >= grafo->totalVertices) {
        printf("Origem invalida.\n");
        return;
    }

    printf("\n--- DFS a partir de %s ---\n", grafo->vertices[origem].nome);
    dfsRecursivo(grafo, origem, visitado);
}

void listarDesabastecidos(const Grafo *grafo, const int visitado[]) {
    int i;
    int encontrouAbastecido = 0;
    int encontrouDesabastecido = 0;

    printf("\nVertices ainda abastecidos:\n");
    for (i = 0; i < grafo->totalVertices; i++) {
        if (visitado[i]) {
            encontrouAbastecido = 1;
            printf("%2d - %s\n", i, grafo->vertices[i].nome);
        }
    }
    if (!encontrouAbastecido) {
        printf("Nenhum vertice abastecido.\n");
    }

    printf("\nVertices desabastecidos ou isolados:\n");
    for (i = 0; i < grafo->totalVertices; i++) {
        if (!visitado[i]) {
            encontrouDesabastecido = 1;
            printf("%2d - %-30s | %s\n", i, grafo->vertices[i].nome,
                   tipoVertice(grafo->vertices[i].nome));
        }
    }
    if (!encontrouDesabastecido) {
        printf("Nenhum vertice ficou desabastecido.\n");
    }
}
