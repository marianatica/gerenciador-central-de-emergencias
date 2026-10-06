#ifndef INTEGRIDADE_H
#define INTEGRIDADE_H

#include "hash.h"

// Modulo 4 - Integridade dos Registros (Modo Investigacao).
// arquivo e o CSV de onde as ocorrencias foram carregadas, usado para comparar com a memoria.
void integridade_menu(TabelaHash *tabela, char *arquivo);

#endif
