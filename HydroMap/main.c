#include <stdio.h>
#include <stdlib.h>

#include "algoritmos.h"
#include "grafo.h"
#include "visualizacao.h"

static void limparEntrada(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }
}

static int lerInteiro(const char *mensagem) {
    int valor;

    printf("%s", mensagem);
    while (scanf("%d", &valor) != 1) {
        printf("Entrada invalida. Digite um numero: ");
        limparEntrada();
    }
    limparEntrada();

    return valor;
}

static void simularFechamento(Grafo *grafo) {
    int origem;
    int destino;
    int visitado[MAX_VERTICES];

    listarTubulacoes(grafo);
    printf("\nEscolha a tubulacao que sera fechada para manutencao.\n");
    origem = lerInteiro("Indice da origem: ");
    destino = lerInteiro("Indice do destino: ");

    if (!bloquearAresta(grafo, origem, destino)) {
        printf("Tubulacao nao encontrada. Nenhuma alteracao foi feita.\n");
        return;
    }

    printf("\nTubulacao fechada: %s -> %s\n",
           grafo->vertices[origem].nome,
           grafo->vertices[destino].nome);

    bfs(grafo, ETA_PRINCIPAL, visitado);
    listarDesabastecidos(grafo, visitado);

    printf("\nA tubulacao permanecera inativa ate ser reativada pelo menu.\n");
}

static void encontrarCaminho(const Grafo *grafo) {
    int origem;
    int destino;
    int caminho[MAX_VERTICES];
    int tamanhoCaminho;
    int custoTotal;
    const char *arquivo = "mapa_hydromap.html";

    listarVertices(grafo);
    printf("\nDica: use origem 0 para calcular a partir da ETA Principal.\n");
    origem = lerInteiro("Origem: ");
    destino = lerInteiro("Destino: ");

    if (dijkstraComCaminho(grafo, origem, destino, caminho,
                           &tamanhoCaminho, &custoTotal)) {
        (void) custoTotal;
        if (gerarMapaHTMLComCaminho(grafo, arquivo, caminho, tamanhoCaminho)) {
            printf("Mapa atualizado com o caminho escolhido em verde: %s\n", arquivo);
        } else {
            printf("Nao foi possivel atualizar o mapa visual.\n");
        }
    }
}

static void reativarTubulacao(Grafo *grafo) {
    int origem;
    int destino;

    listarTubulacoes(grafo);
    printf("\nEscolha a tubulacao que sera reativada.\n");
    origem = lerInteiro("Indice da origem: ");
    destino = lerInteiro("Indice do destino: ");

    if (reativarAresta(grafo, origem, destino)) {
        printf("Tubulacao reativada: %s -> %s\n",
               grafo->vertices[origem].nome,
               grafo->vertices[destino].nome);
    } else {
        printf("Tubulacao nao encontrada.\n");
    }
}

static void exportarMapaVisual(const Grafo *grafo) {
    const char *arquivo = "mapa_hydromap.html";

    if (gerarMapaHTML(grafo, arquivo)) {
        printf("\nMapa visual gerado com sucesso: %s\n", arquivo);
        printf("Abra esse arquivo no navegador para ver a rede em formato grafico.\n");
    } else {
        printf("\nErro ao gerar o mapa visual.\n");
    }
}

static void exibirMenu(void) {
    printf("\n===== HydroMap - Gestao de Rede de Agua e Saneamento =====\n\n");
    printf("1 - Exibir vertices da rede\n");
    printf("2 - Exibir tubulacoes cadastradas\n");
    printf("3 - Encontrar caminho mais eficiente\n");
    printf("4 - Simular fechamento de tubulacao\n");
    printf("5 - Executar BFS a partir da ETA\n");
    printf("6 - Executar DFS a partir da ETA\n");
    printf("7 - Exibir rede completa\n");
    printf("8 - Reativar tubulacao\n");
    printf("9 - Gerar interface grafica do mapa\n");
    printf("0 - Sair\n");
}

int main(void) {
    Grafo grafo;
    int opcao;

    inicializarGrafo(&grafo);
    cadastrarRedeHydroMap(&grafo);

    do {
        exibirMenu();
        opcao = lerInteiro("Escolha uma opcao: ");

        switch (opcao) {
            case 1:
                listarVertices(&grafo);
                break;
            case 2:
                listarTubulacoes(&grafo);
                break;
            case 3:
                encontrarCaminho(&grafo);
                break;
            case 4:
                simularFechamento(&grafo);
                break;
            case 5:
                executarBFS(&grafo, ETA_PRINCIPAL);
                break;
            case 6:
                executarDFS(&grafo, ETA_PRINCIPAL);
                break;
            case 7:
                imprimirRedeCompleta(&grafo);
                break;
            case 8:
                reativarTubulacao(&grafo);
                break;
            case 9:
                exportarMapaVisual(&grafo);
                break;
            case 0:
                printf("Encerrando o HydroMap.\n");
                break;
            default:
                printf("Opcao invalida. Tente novamente.\n");
                break;
        }
    } while (opcao != 0);

    liberarGrafo(&grafo);
    return 0;
}
