# HydroMap

## Tema

Gestao de Rede de Agua e Saneamento usando grafos em linguagem C.

## Objetivo

O HydroMap simula uma rede municipal de distribuicao de agua potavel e coleta de esgoto. O sistema permite visualizar a rede, calcular o caminho hidraulico mais eficiente, simular o fechamento de uma tubulacao para manutencao e gerar uma interface grafica em HTML/SVG.

## Representacao do grafo

A rede foi modelada como um grafo dirigido e ponderado:

- Vertices: pontos importantes da rede, como ETA, reservatorios, bombas, bairros e estacoes.
- Arestas dirigidas: tubulacoes ou adutoras com sentido de fluxo da agua.
- Pesos: distancia da tubulacao ou perda de carga hidraulica.
- Status da aresta: ativa ou inativa, permitindo simular manutencao.

O projeto usa lista de adjacencia, porque esse formato representa bem redes reais em que cada ponto se conecta apenas a alguns outros pontos.

## Vertices cadastrados

O grafo possui 21 vertices fixos:

0. ETA Principal
1. Reservatorio Central
2. Reservatorio Norte
3. Reservatorio Sul
4. Reservatorio Leste
5. Reservatorio Oeste
6. Bomba Zona Norte
7. Bomba Zona Sul
8. Bomba Zona Leste
9. Bomba Zona Oeste
10. Bairro Centro
11. Bairro Industrial
12. Bairro Aeroporto
13. Bairro Universitario
14. Bairro Jardim das Aguas
15. Bairro Sao Cristovao
16. Bairro Boa Esperanca
17. Bairro Vila Nova
18. Estacao Elevatoria 1
19. Estacao Elevatoria 2
20. Estacao de Esgoto Principal

## Algoritmos usados

### Dijkstra

Calcula o caminho de menor custo entre dois vertices. O algoritmo considera apenas tubulacoes ativas. No HydroMap, ele pode ser usado para encontrar o caminho mais eficiente da ETA Principal ate um reservatorio ou bairro.

Ao usar a opcao 3 do menu, o sistema tambem atualiza automaticamente o arquivo `mapa_hydromap.html`, destacando o caminho encontrado com setas verdes.

### BFS

Percorre o grafo em largura. A partir da ETA Principal, marca todos os vertices que ainda recebem fluxo. Depois de fechar uma tubulacao, os vertices nao visitados pela BFS sao listados como desabastecidos ou isolados.

### DFS

Percorre o grafo em profundidade. O DFS ajuda a demonstrar como a rede pode ser explorada seguindo um caminho ate o fim antes de voltar para outros ramos.

## Interface grafica

O projeto inclui uma visualizacao grafica simples sem bibliotecas externas. Pelo menu, a opcao 9 gera o arquivo `mapa_hydromap.html`. Esse arquivo pode ser aberto no navegador e mostra:

- vertices coloridos por tipo;
- setas indicando o sentido do fluxo;
- pesos das tubulacoes;
- tubulacoes ativas em azul;
- tubulacoes inativas em vermelho tracejado.
- caminho escolhido pelo Dijkstra em verde.
```

## Exemplo de uso

Para encontrar o caminho mais eficiente da ETA Principal ate o Reservatorio Norte:

```text
Origem: 0
Destino: 2

--- Caminho mais eficiente ---
Origem : ETA Principal
Destino: Reservatorio Norte
Custo total: 210
Caminho: ETA Principal -> Reservatorio Central -> Reservatorio Norte
Mapa atualizado com o caminho escolhido em verde: mapa_hydromap.html
```

Para simular manutencao, escolha uma tubulacao pelo indice de origem e destino. Exemplo:

```text
Indice da origem: 1
Indice do destino: 2

Tubulacao fechada: Reservatorio Central -> Reservatorio Norte

Vertices ainda abastecidos:
0 - ETA Principal
1 - Reservatorio Central
...

Vertices desabastecidos ou isolados:
2 - Reservatorio Norte              | Reservatorio
6 - Bomba Zona Norte                | Bomba
...
```

Para gerar a interface grafica:

```text
Escolha uma opcao: 9

Mapa visual gerado com sucesso: mapa_hydromap.html
Abra esse arquivo no navegador para ver a rede em formato grafico.
```

