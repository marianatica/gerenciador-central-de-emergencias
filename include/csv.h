#ifndef CSV_H
#define CSV_H

#include "ocorrencia.h"

// Lê o CSV do dataset 911 Calls e preenche o vetor (no máximo `max` ocorrências).
// Retorna quantas foram lidas, ou -1 se o arquivo não abriu.
// Linhas com formato inválido são ignoradas, com um aviso na tela.
int csv_carregar(const char *caminho, Ocorrencia *vetor, int max);

#endif
