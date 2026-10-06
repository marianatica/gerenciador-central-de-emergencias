#ifndef INTEGRIDADE_H
#define INTEGRIDADE_H

#include "hash.h"

// Módulo 4 – Integridade dos Registros (Modo Investigação).
// arquivo é o CSV de onde as ocorrências foram carregadas, usado para comparar com a memória.
void integridade_menu(TabelaHash *tabela, char *arquivo);

#endif
