#ifndef CSV_H
#define CSV_H

#include "ocorrencia.h"

// Lê o CSV do dataset 911 Calls e preenche o vetor (no máximo `max` ocorrências).
// Retorna quantas foram lidas, ou -1 se o arquivo não abriu.
// Se `descartadas` não for NULL, recebe quantas linhas inválidas foram ignoradas.
int csv_carregar(const char *caminho, Ocorrencia *vetor, int max, int *descartadas);

#endif
