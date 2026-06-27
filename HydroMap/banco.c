#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "banco.h"

#ifndef USE_ODBC

void testarConexaoBanco(void) {
    printf("\nConexao com banco desativada nesta compilacao.\n");
    printf("Para habilitar ODBC no Windows, compile com:\n");
    printf("gcc -DUSE_ODBC main.c grafo.c algoritmos.c visualizacao.c banco.c -o hydromap -lodbc32\n");
    printf("O DSN esperado e: %s\n", HYDRA_DSN_PADRAO);
}

void sincronizarRedeBanco(const Grafo *grafo) {
    (void) grafo;
    testarConexaoBanco();
}

#else

#include <windows.h>
#include <sql.h>
#include <sqlext.h>

typedef struct {
    SQLHENV ambiente;
    SQLHDBC conexao;
    int conectado;
} ConexaoBanco;

static const char *obterDSN(void) {
    const char *dsn = getenv("HYDRA_DSN");

    if (dsn == NULL || dsn[0] == '\0') {
        return HYDRA_DSN_PADRAO;
    }

    return dsn;
}

static void mostrarErroODBC(SQLSMALLINT tipoHandle, SQLHANDLE handle, const char *acao) {
    SQLCHAR estado[6];
    SQLCHAR mensagem[256];
    SQLINTEGER erroNativo;
    SQLSMALLINT tamanho;
    SQLRETURN retorno;

    retorno = SQLGetDiagRec(tipoHandle, handle, 1, estado, &erroNativo,
                            mensagem, sizeof(mensagem), &tamanho);

    printf("Erro ao %s.\n", acao);
    if (SQL_SUCCEEDED(retorno)) {
        printf("ODBC %s | Codigo %ld | %s\n",
               (char *) estado, (long) erroNativo, (char *) mensagem);
    }
}

static int executarSQL(SQLHDBC conexao, const char *sql) {
    SQLHSTMT comando;
    SQLRETURN retorno;

    retorno = SQLAllocHandle(SQL_HANDLE_STMT, conexao, &comando);
    if (!SQL_SUCCEEDED(retorno)) {
        mostrarErroODBC(SQL_HANDLE_DBC, conexao, "preparar comando SQL");
        return 0;
    }

    retorno = SQLExecDirect(comando, (SQLCHAR *) sql, SQL_NTS);
    if (!SQL_SUCCEEDED(retorno)) {
        mostrarErroODBC(SQL_HANDLE_STMT, comando, "executar comando SQL");
        SQLFreeHandle(SQL_HANDLE_STMT, comando);
        return 0;
    }

    SQLFreeHandle(SQL_HANDLE_STMT, comando);
    return 1;
}

static int conectarBanco(ConexaoBanco *banco) {
    SQLRETURN retorno;
    char stringConexao[128];

    banco->ambiente = SQL_NULL_HENV;
    banco->conexao = SQL_NULL_HDBC;
    banco->conectado = 0;

    retorno = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &banco->ambiente);
    if (!SQL_SUCCEEDED(retorno)) {
        printf("Erro ao criar ambiente ODBC.\n");
        return 0;
    }

    retorno = SQLSetEnvAttr(banco->ambiente, SQL_ATTR_ODBC_VERSION,
                            (void *) SQL_OV_ODBC3, 0);
    if (!SQL_SUCCEEDED(retorno)) {
        mostrarErroODBC(SQL_HANDLE_ENV, banco->ambiente, "configurar ODBC");
        SQLFreeHandle(SQL_HANDLE_ENV, banco->ambiente);
        return 0;
    }

    retorno = SQLAllocHandle(SQL_HANDLE_DBC, banco->ambiente, &banco->conexao);
    if (!SQL_SUCCEEDED(retorno)) {
        mostrarErroODBC(SQL_HANDLE_ENV, banco->ambiente, "criar conexao");
        SQLFreeHandle(SQL_HANDLE_ENV, banco->ambiente);
        return 0;
    }

    snprintf(stringConexao, sizeof(stringConexao), "DSN=%s;", obterDSN());
    retorno = SQLDriverConnect(banco->conexao, NULL, (SQLCHAR *) stringConexao,
                               SQL_NTS, NULL, 0, NULL, SQL_DRIVER_NOPROMPT);
    if (!SQL_SUCCEEDED(retorno)) {
        mostrarErroODBC(SQL_HANDLE_DBC, banco->conexao, "conectar ao banco hydrabanco");
        SQLFreeHandle(SQL_HANDLE_DBC, banco->conexao);
        SQLFreeHandle(SQL_HANDLE_ENV, banco->ambiente);
        return 0;
    }

    banco->conectado = 1;
    return 1;
}

static void desconectarBanco(ConexaoBanco *banco) {
    if (banco->conectado) {
        SQLDisconnect(banco->conexao);
    }
    if (banco->conexao != SQL_NULL_HDBC) {
        SQLFreeHandle(SQL_HANDLE_DBC, banco->conexao);
    }
    if (banco->ambiente != SQL_NULL_HENV) {
        SQLFreeHandle(SQL_HANDLE_ENV, banco->ambiente);
    }
}

static void escaparSQL(const char *origem, char *destino, size_t tamanhoDestino) {
    size_t i;
    size_t j = 0;

    for (i = 0; origem[i] != '\0' && j + 1 < tamanhoDestino; i++) {
        if (origem[i] == '\'' && j + 2 < tamanhoDestino) {
            destino[j++] = '\'';
            destino[j++] = '\'';
        } else {
            destino[j++] = origem[i];
        }
    }
    destino[j] = '\0';
}

static int criarTabelas(SQLHDBC conexao) {
    return executarSQL(conexao,
                       "CREATE TABLE IF NOT EXISTS vertices ("
                       "id INTEGER PRIMARY KEY,"
                       "nome VARCHAR(100) NOT NULL,"
                       "tipo VARCHAR(60) NOT NULL"
                       ")")
        && executarSQL(conexao,
                       "CREATE TABLE IF NOT EXISTS tubulacoes ("
                       "origem INTEGER NOT NULL,"
                       "destino INTEGER NOT NULL,"
                       "peso INTEGER NOT NULL,"
                       "ativa INTEGER NOT NULL,"
                       "PRIMARY KEY (origem, destino)"
                       ")");
}

static int limparTabelas(SQLHDBC conexao) {
    return executarSQL(conexao, "DELETE FROM tubulacoes")
        && executarSQL(conexao, "DELETE FROM vertices");
}

static int inserirVertices(SQLHDBC conexao, const Grafo *grafo) {
    int i;
    char sql[512];
    char nome[TAM_NOME * 2];
    char tipo[80];

    for (i = 0; i < grafo->totalVertices; i++) {
        escaparSQL(grafo->vertices[i].nome, nome, sizeof(nome));
        escaparSQL(tipoVertice(grafo->vertices[i].nome), tipo, sizeof(tipo));
        snprintf(sql, sizeof(sql),
                 "INSERT INTO vertices (id, nome, tipo) VALUES (%d, '%s', '%s')",
                 i, nome, tipo);

        if (!executarSQL(conexao, sql)) {
            return 0;
        }
    }

    return 1;
}

static int inserirTubulacoes(SQLHDBC conexao, const Grafo *grafo) {
    int origem;
    char sql[256];
    Aresta *aresta;

    for (origem = 0; origem < grafo->totalVertices; origem++) {
        aresta = grafo->vertices[origem].lista;
        while (aresta != NULL) {
            snprintf(sql, sizeof(sql),
                     "INSERT INTO tubulacoes (origem, destino, peso, ativa) "
                     "VALUES (%d, %d, %d, %d)",
                     origem, aresta->destino, aresta->peso, aresta->ativa);

            if (!executarSQL(conexao, sql)) {
                return 0;
            }

            aresta = aresta->proxima;
        }
    }

    return 1;
}

void testarConexaoBanco(void) {
    ConexaoBanco banco;

    printf("\nTestando conexao ODBC com DSN '%s'...\n", obterDSN());
    if (conectarBanco(&banco)) {
        printf("Conexao realizada com sucesso.\n");
        desconectarBanco(&banco);
    }
}

void sincronizarRedeBanco(const Grafo *grafo) {
    ConexaoBanco banco;

    printf("\nSincronizando rede HydroMap com o banco '%s'...\n", obterDSN());
    if (!conectarBanco(&banco)) {
        return;
    }

    if (criarTabelas(banco.conexao) &&
        limparTabelas(banco.conexao) &&
        inserirVertices(banco.conexao, grafo) &&
        inserirTubulacoes(banco.conexao, grafo)) {
        printf("Rede sincronizada com sucesso no banco.\n");
    } else {
        printf("Nao foi possivel concluir a sincronizacao.\n");
    }

    desconectarBanco(&banco);
}

#endif
