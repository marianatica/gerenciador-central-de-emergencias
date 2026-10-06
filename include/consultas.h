#ifndef CONSULTAS_H
#define CONSULTAS_H

#include "hash.h"
#include "indices.h"

// Modulo 2 - Consulta Rapida: busca por id, descricao, tipo, regiao, prefixo e intervalo de datas.
void consulta_rapida_menu(TabelaHash *tabela, Indices *indices);

// Modulo 3 - Organizacao das Ocorrencias: filtros por regiao e categoria e listagens em ordem.
void organizacao_menu(TabelaHash *tabela, Indices *indices);

#endif
