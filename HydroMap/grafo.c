#include <stdio.h>
#include <string.h>

#include "grafo.h"

void inicializarGrafo(Grafo *grafo) {
    int i;
    int j;

    grafo->totalVertices = MAX_VERTICES;
    for (i = 0; i < MAX_VERTICES; i++) {
        grafo->vertices[i].nome[0] = '\0';
        for (j = 0; j < MAX_VERTICES; j++) {
            grafo->matrizPesos[i][j] = 0;
            grafo->matrizAtiva[i][j] = 0;
        }
    }
}

void adicionarVertice(Grafo *grafo, int indice, const char *nome) {
    if (indice < 0 || indice >= MAX_VERTICES) {
        return;
    }

    strncpy(grafo->vertices[indice].nome, nome, TAM_NOME - 1);
    grafo->vertices[indice].nome[TAM_NOME - 1] = '\0';
}

void adicionarAresta(Grafo *grafo, int origem, int destino, int peso) {
    if (origem < 0 || origem >= grafo->totalVertices ||
        destino < 0 || destino >= grafo->totalVertices) {
        return;
    }

    grafo->matrizPesos[origem][destino] = peso;
    grafo->matrizAtiva[origem][destino] = 1;
}

void cadastrarRedeHydroMap(Grafo *grafo) {
    adicionarVertice(grafo, 0, "ETA Principal");
    adicionarVertice(grafo, 1, "Reservatorio Central");
    adicionarVertice(grafo, 2, "Reservatorio Norte");
    adicionarVertice(grafo, 3, "Reservatorio Sul");
    adicionarVertice(grafo, 4, "Reservatorio Leste");
    adicionarVertice(grafo, 5, "Reservatorio Oeste");
    adicionarVertice(grafo, 6, "Bomba Zona Norte");
    adicionarVertice(grafo, 7, "Bomba Zona Sul");
    adicionarVertice(grafo, 8, "Bomba Zona Leste");
    adicionarVertice(grafo, 9, "Bomba Zona Oeste");
    adicionarVertice(grafo, 10, "Bairro Centro");
    adicionarVertice(grafo, 11, "Bairro Industrial");
    adicionarVertice(grafo, 12, "Bairro Aeroporto");
    adicionarVertice(grafo, 13, "Bairro Universitario");
    adicionarVertice(grafo, 14, "Bairro Jardim das Aguas");
    adicionarVertice(grafo, 15, "Bairro Sao Cristovao");
    adicionarVertice(grafo, 16, "Bairro Boa Esperanca");
    adicionarVertice(grafo, 17, "Bairro Vila Nova");
    adicionarVertice(grafo, 18, "Estacao Elevatoria 1");
    adicionarVertice(grafo, 19, "Estacao Elevatoria 2");
    adicionarVertice(grafo, 20, "Estacao de Esgoto Principal");

    /*
     * Cadastro fixo da rede.
     * O grafo e dirigido porque a agua tem sentido definido de fluxo.
     * Cada peso pode ser lido como metros de tubulacao ou perda de carga.
     */
    adicionarAresta(grafo, 0, 1, 120);
    adicionarAresta(grafo, 0, 18, 140);
    adicionarAresta(grafo, 1, 2, 90);
    adicionarAresta(grafo, 1, 3, 95);
    adicionarAresta(grafo, 1, 4, 110);
    adicionarAresta(grafo, 1, 5, 105);
    adicionarAresta(grafo, 1, 10, 45);
    adicionarAresta(grafo, 2, 6, 50);
    adicionarAresta(grafo, 3, 7, 55);
    adicionarAresta(grafo, 4, 8, 60);
    adicionarAresta(grafo, 5, 9, 58);
    adicionarAresta(grafo, 6, 11, 70);
    adicionarAresta(grafo, 6, 13, 65);
    adicionarAresta(grafo, 7, 12, 80);
    adicionarAresta(grafo, 7, 16, 75);
    adicionarAresta(grafo, 8, 14, 68);
    adicionarAresta(grafo, 8, 19, 72);
    adicionarAresta(grafo, 9, 15, 62);
    adicionarAresta(grafo, 9, 17, 85);
    adicionarAresta(grafo, 18, 3, 100);
    adicionarAresta(grafo, 19, 5, 115);
    adicionarAresta(grafo, 10, 20, 130);
    adicionarAresta(grafo, 11, 20, 150);
    adicionarAresta(grafo, 12, 20, 160);
    adicionarAresta(grafo, 13, 20, 145);
    adicionarAresta(grafo, 14, 20, 155);
    adicionarAresta(grafo, 15, 20, 135);
    adicionarAresta(grafo, 16, 20, 142);
    adicionarAresta(grafo, 17, 20, 148);
}

void listarVertices(const Grafo *grafo) {
    int i;

    printf("\n--- Vertices da rede HydroMap ---\n");
    for (i = 0; i < grafo->totalVertices; i++) {
        printf("%2d - %s (%s)\n", i, grafo->vertices[i].nome,
               tipoVertice(grafo->vertices[i].nome));
    }
}

void listarTubulacoes(const Grafo *grafo) {
    int origem;
    int destino;

    printf("\n--- Tubulacoes cadastradas ---\n");
    for (origem = 0; origem < grafo->totalVertices; origem++) {
        for (destino = 0; destino < grafo->totalVertices; destino++) {
            if (grafo->matrizPesos[origem][destino] == 0) {
                continue;
            }

            printf("%2d %-30s -> %2d %-30s | peso: %3d | %s\n",
                   origem,
                   grafo->vertices[origem].nome,
                   destino,
                   grafo->vertices[destino].nome,
                   grafo->matrizPesos[origem][destino],
                   grafo->matrizAtiva[origem][destino] ? "ativa" : "inativa");
        }
    }
}

void imprimirRedeCompleta(const Grafo *grafo) {
    int origem;
    int destino;
    int temSaida;

    printf("\n--- Rede completa em matriz de adjacencia ---\n");
    for (origem = 0; origem < grafo->totalVertices; origem++) {
        temSaida = 0;
        printf("%2d - %-30s: ", origem, grafo->vertices[origem].nome);

        for (destino = 0; destino < grafo->totalVertices; destino++) {
            if (grafo->matrizPesos[origem][destino] == 0) {
                continue;
            }

            temSaida = 1;
            printf("-> [%s, peso %d, %s] ",
                   grafo->vertices[destino].nome,
                   grafo->matrizPesos[origem][destino],
                   grafo->matrizAtiva[origem][destino] ? "ativa" : "inativa");
        }

        if (!temSaida) {
            printf("sem saidas");
        }
        printf("\n");
    }
}

int bloquearAresta(Grafo *grafo, int origem, int destino) {
    if (origem < 0 || origem >= grafo->totalVertices ||
        destino < 0 || destino >= grafo->totalVertices) {
        return 0;
    }

    if (grafo->matrizPesos[origem][destino] == 0) {
        return 0;
    }

    grafo->matrizAtiva[origem][destino] = 0;
    return 1;
}

int reativarAresta(Grafo *grafo, int origem, int destino) {
    if (origem < 0 || origem >= grafo->totalVertices ||
        destino < 0 || destino >= grafo->totalVertices) {
        return 0;
    }

    if (grafo->matrizPesos[origem][destino] == 0) {
        return 0;
    }

    grafo->matrizAtiva[origem][destino] = 1;
    return 1;
}

void liberarGrafo(Grafo *grafo) {
    (void) grafo;
}

const char *tipoVertice(const char *nome) {
    if (strncmp(nome, "ETA", 3) == 0) {
        return "Estacao de Tratamento de Agua";
    }
    if (strncmp(nome, "Reservatorio", 12) == 0) {
        return "Reservatorio";
    }
    if (strncmp(nome, "Bomba", 5) == 0) {
        return "Bomba";
    }
    if (strncmp(nome, "Bairro", 6) == 0) {
        return "Bairro";
    }
    if (strncmp(nome, "Estacao de Esgoto", 17) == 0) {
        return "Estacao de Esgoto";
    }
    return "Estacao Elevatoria";
}
