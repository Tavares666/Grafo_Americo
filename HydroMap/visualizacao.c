#include <stdio.h>
#include <string.h>

#include "visualizacao.h"

typedef struct {
    int x;
    int y;
} Ponto;

static const Ponto posicoes[MAX_VERTICES] = {
    {80, 310}, {260, 310}, {470, 100}, {470, 500}, {470, 300},
    {470, 690}, {680, 100}, {680, 500}, {680, 300}, {680, 690},
    {470, 850}, {910, 55}, {910, 450}, {910, 145}, {910, 255},
    {910, 635}, {910, 545}, {910, 725}, {260, 500}, {680, 390},
    {1150, 390}
};

static const char *classeVertice(const char *nome) {
    if (strncmp(nome, "ETA", 3) == 0) {
        return "eta";
    }
    if (strncmp(nome, "Reservatorio", 12) == 0) {
        return "reservatorio";
    }
    if (strncmp(nome, "Bomba", 5) == 0) {
        return "bomba";
    }
    if (strncmp(nome, "Bairro", 6) == 0) {
        return "bairro";
    }
    if (strncmp(nome, "Estacao de Esgoto", 17) == 0) {
        return "esgoto";
    }
    return "elevatoria";
}

static void escreverCabecalho(FILE *arquivo, int temCaminho) {
    fprintf(arquivo, "<!DOCTYPE html>\n<html lang=\"pt-BR\">\n<head>\n");
    fprintf(arquivo, "<meta charset=\"UTF-8\">\n");
    fprintf(arquivo, "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n");
    fprintf(arquivo, "<title>HydroMap - Mapa da Rede</title>\n");
    fprintf(arquivo, "<style>\n");
    fprintf(arquivo, "body{margin:0;font-family:Arial,sans-serif;background:#eef5f8;color:#16313a;}\n");
    fprintf(arquivo, "header{padding:18px 28px;background:#173f4f;color:white;}h1{margin:0;font-size:26px;}p{margin:6px 0 0;}\n");
    fprintf(arquivo, ".legenda{display:flex;gap:18px;flex-wrap:wrap;padding:14px 28px;background:#fff;border-bottom:1px solid #c9d7dd;}\n");
    fprintf(arquivo, ".item{display:flex;align-items:center;gap:7px;font-size:14px;}.amostra{width:16px;height:16px;border-radius:50%%;display:inline-block;border:2px solid #244;}\n");
    fprintf(arquivo, ".mapa{padding:18px;overflow:auto;}svg{background:#fdfefe;border:1px solid #c9d7dd;box-shadow:0 2px 8px #0002;}\n");
    fprintf(arquivo, ".aresta{stroke:#2477a8;stroke-width:3;fill:none;marker-end:url(#seta);}.inativa{stroke:#c84343;stroke-dasharray:8 6;marker-end:url(#setaInativa);}.caminho{stroke:#2f9e44;stroke-width:6;marker-end:url(#setaCaminho);}\n");
    fprintf(arquivo, ".peso{font-size:13px;fill:#314b55;font-weight:bold;paint-order:stroke;stroke:#fff;stroke-width:4px;}\n");
    fprintf(arquivo, ".no circle{stroke:#173f4f;stroke-width:2;}.no text{font-size:12px;fill:#102c35;text-anchor:middle;font-weight:bold;paint-order:stroke;stroke:#fff;stroke-width:3px;}\n");
    fprintf(arquivo, ".eta circle{fill:#2f9e44;}.reservatorio circle{fill:#4dabf7;}.bomba circle{fill:#f59f00;}.bairro circle{fill:#d0bfff;}.elevatoria circle{fill:#63e6be;}.esgoto circle{fill:#868e96;}\n");
    fprintf(arquivo, ".matriz{padding:0 28px 28px;background:#eef5f8;}.matriz h2{margin:0 0 8px;font-size:22px;color:#173f4f;}.matriz p{margin:0 0 14px;color:#314b55;}.tabela-wrap{overflow:auto;background:#fff;border:1px solid #c9d7dd;box-shadow:0 2px 8px #0001;}table{border-collapse:collapse;min-width:980px;width:100%%;font-size:12px;}th,td{border:1px solid #d7e3e8;padding:7px;text-align:center;white-space:nowrap;}th{background:#173f4f;color:#fff;position:sticky;top:0;}th:first-child{left:0;z-index:2;}td:first-child{background:#f3f8fa;font-weight:bold;text-align:left;position:sticky;left:0;}td.valor{background:#d9f6df;color:#1f6b31;font-weight:bold;}td.inativa-matriz{background:#ffe2e2;color:#9b2626;font-weight:bold;text-decoration:line-through;}td.zero{color:#91a3aa;}\n");
    fprintf(arquivo, "</style>\n</head>\n<body>\n");
    fprintf(arquivo, "<header><h1>HydroMap</h1><p>Mapa visual da rede dirigida e ponderada de agua e saneamento</p></header>\n");
    fprintf(arquivo, "<section class=\"legenda\">\n");
    fprintf(arquivo, "<span class=\"item\"><span class=\"amostra\" style=\"background:#2f9e44\"></span>ETA</span>\n");
    fprintf(arquivo, "<span class=\"item\"><span class=\"amostra\" style=\"background:#4dabf7\"></span>Reservatorio</span>\n");
    fprintf(arquivo, "<span class=\"item\"><span class=\"amostra\" style=\"background:#f59f00\"></span>Bomba</span>\n");
    fprintf(arquivo, "<span class=\"item\"><span class=\"amostra\" style=\"background:#d0bfff\"></span>Bairro</span>\n");
    fprintf(arquivo, "<span class=\"item\"><span class=\"amostra\" style=\"background:#63e6be\"></span>Estacao elevatoria</span>\n");
    fprintf(arquivo, "<span class=\"item\"><span class=\"amostra\" style=\"background:#868e96\"></span>Esgoto</span>\n");
    fprintf(arquivo, "<span class=\"item\">Linha azul: ativa</span><span class=\"item\">Linha vermelha tracejada: inativa</span><span class=\"item\">Linha verde: caminho escolhido</span>\n");
    if (temCaminho) {
        fprintf(arquivo, "<span class=\"item\"><strong>Caminho calculado pelo Dijkstra destacado em verde</strong></span>\n");
    }
    fprintf(arquivo, "</section>\n<section class=\"mapa\">\n");
    fprintf(arquivo, "<svg width=\"1240\" height=\"920\" viewBox=\"0 0 1240 920\" xmlns=\"http://www.w3.org/2000/svg\">\n");
    fprintf(arquivo, "<defs><marker id=\"seta\" markerWidth=\"12\" markerHeight=\"12\" refX=\"10\" refY=\"3\" orient=\"auto\" markerUnits=\"strokeWidth\"><path d=\"M0,0 L0,6 L10,3 z\" fill=\"#2477a8\"/></marker>");
    fprintf(arquivo, "<marker id=\"setaInativa\" markerWidth=\"12\" markerHeight=\"12\" refX=\"10\" refY=\"3\" orient=\"auto\" markerUnits=\"strokeWidth\"><path d=\"M0,0 L0,6 L10,3 z\" fill=\"#c84343\"/></marker>");
    fprintf(arquivo, "<marker id=\"setaCaminho\" markerWidth=\"12\" markerHeight=\"12\" refX=\"10\" refY=\"3\" orient=\"auto\" markerUnits=\"strokeWidth\"><path d=\"M0,0 L0,6 L10,3 z\" fill=\"#2f9e44\"/></marker></defs>\n");
}

static int arestaEstaNoCaminho(int origem, int destino,
                               const int caminho[], int tamanhoCaminho) {
    int i;

    for (i = 0; i < tamanhoCaminho - 1; i++) {
        if (caminho[i] == origem && caminho[i + 1] == destino) {
            return 1;
        }
    }

    return 0;
}

static void escreverArestas(FILE *arquivo, const Grafo *grafo,
                            const int caminho[], int tamanhoCaminho) {
    int origem;
    Aresta *aresta;

    for (origem = 0; origem < grafo->totalVertices; origem++) {
        aresta = grafo->vertices[origem].lista;
        while (aresta != NULL) {
            int x1 = posicoes[origem].x;
            int y1 = posicoes[origem].y;
            int x2 = posicoes[aresta->destino].x;
            int y2 = posicoes[aresta->destino].y;
            int emCaminho = arestaEstaNoCaminho(origem, aresta->destino,
                                                caminho, tamanhoCaminho);

            fprintf(arquivo,
                    "<line class=\"aresta%s\" x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\"><title>%s -> %s | peso %d | %s</title></line>\n",
                    emCaminho ? " caminho" : (aresta->ativa ? "" : " inativa"),
                    x1, y1, x2, y2,
                    grafo->vertices[origem].nome,
                    grafo->vertices[aresta->destino].nome,
                    aresta->peso,
                    aresta->ativa ? "ativa" : "inativa");
            fprintf(arquivo, "<text class=\"peso\" x=\"%d\" y=\"%d\">%d</text>\n",
                    (x1 + x2) / 2, (y1 + y2) / 2 - 6, aresta->peso);
            aresta = aresta->proxima;
        }
    }
}

static void escreverVertices(FILE *arquivo, const Grafo *grafo) {
    int i;

    for (i = 0; i < grafo->totalVertices; i++) {
        fprintf(arquivo,
                "<g class=\"no %s\"><circle cx=\"%d\" cy=\"%d\" r=\"24\"><title>%d - %s</title></circle><text x=\"%d\" y=\"%d\">%d</text><text x=\"%d\" y=\"%d\">%s</text></g>\n",
                classeVertice(grafo->vertices[i].nome),
                posicoes[i].x, posicoes[i].y,
                i, grafo->vertices[i].nome,
                posicoes[i].x, posicoes[i].y + 5, i,
                posicoes[i].x, posicoes[i].y + 42,
                grafo->vertices[i].nome);
    }
}

static void escreverMatrizAdjacencia(FILE *arquivo, const Grafo *grafo) {
    int origem;
    int destino;

    fprintf(arquivo, "<section class=\"matriz\">\n");
    fprintf(arquivo, "<h2>Matriz de adjacencia</h2>\n");
    fprintf(arquivo, "<p>Linhas indicam a origem, colunas indicam o destino. O valor mostra o peso da ligacao; zero significa que nao existe ligacao direta.</p>\n");
    fprintf(arquivo, "<div class=\"tabela-wrap\">\n<table>\n<thead><tr><th>Origem / Destino</th>");

    for (destino = 0; destino < grafo->totalVertices; destino++) {
        fprintf(arquivo, "<th>%d</th>", destino);
    }

    fprintf(arquivo, "</tr></thead>\n<tbody>\n");

    for (origem = 0; origem < grafo->totalVertices; origem++) {
        fprintf(arquivo, "<tr><td>%d %s</td>", origem, grafo->vertices[origem].nome);

        for (destino = 0; destino < grafo->totalVertices; destino++) {
            Aresta *aresta = grafo->vertices[origem].lista;
            int peso = 0;
            int ativa = 0;

            while (aresta != NULL) {
                if (aresta->destino == destino) {
                    peso = aresta->peso;
                    ativa = aresta->ativa;
                    break;
                }
                aresta = aresta->proxima;
            }

            if (peso > 0) {
                fprintf(arquivo, "<td class=\"%s\">%d</td>",
                        ativa ? "valor" : "inativa-matriz", peso);
            } else {
                fprintf(arquivo, "<td class=\"zero\">0</td>");
            }
        }

        fprintf(arquivo, "</tr>\n");
    }

    fprintf(arquivo, "</tbody>\n</table>\n</div>\n</section>\n");
}

int gerarMapaHTML(const Grafo *grafo, const char *nomeArquivo) {
    return gerarMapaHTMLComCaminho(grafo, nomeArquivo, NULL, 0);
}

int gerarMapaHTMLComCaminho(const Grafo *grafo, const char *nomeArquivo,
                            const int caminho[], int tamanhoCaminho) {
    FILE *arquivo = fopen(nomeArquivo, "w");

    if (arquivo == NULL) {
        return 0;
    }

    escreverCabecalho(arquivo, tamanhoCaminho > 1);
    escreverArestas(arquivo, grafo, caminho, tamanhoCaminho);
    escreverVertices(arquivo, grafo);
    fprintf(arquivo, "</svg>\n</section>\n");
    escreverMatrizAdjacencia(arquivo, grafo);
    fprintf(arquivo, "</body>\n</html>\n");
    fclose(arquivo);

    return 1;
}
