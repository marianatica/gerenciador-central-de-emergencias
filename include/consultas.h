#ifndef CONSULTAS_H
#define CONSULTAS_H

#include "hash.h"
#include "indices.h"

// Módulo 2 – Consulta Rápida: busca por id, descrição, tipo, região, prefixo e intervalo de datas.
void consulta_rapida_menu(TabelaHash *tabela, Indices *indices);

// Módulo 3 – Organização das Ocorrências: filtros por região e categoria e listagens em ordem.
void organizacao_menu(TabelaHash *tabela, Indices *indices);

#endif
