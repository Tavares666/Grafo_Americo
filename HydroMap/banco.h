#ifndef BANCO_H
#define BANCO_H

#include "grafo.h"

#define HYDRA_DSN_PADRAO "hydrabanco"

void testarConexaoBanco(void);
void sincronizarRedeBanco(const Grafo *grafo);

#endif
