#ifndef CSV_H
#define CSV_H

#include "ocorrencia.h"

// Le o CSV do dataset 911 Calls e preenche o vetor (no maximo `max` ocorrencias).
int csv_carregar(const char *caminho, Ocorrencia *vetor, int max);

#endif
