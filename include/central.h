#ifndef CENTRAL_H
#define CENTRAL_H

#include "hash.h"
#include "indices.h"

// Modulo 1 - Central de Ocorrencias: cadastrar, consultar, alterar, remover e listar.
// proximo_id e o id que a proxima ocorrencia cadastrada vai receber.
void central_menu(TabelaHash *tabela, Indices *indices, int *proximo_id);

// Mostra todos os campos de uma ocorrencia
void central_mostrar_ocorrencia(Ocorrencia *o);

// Mostra uma linha resumida para cada id da lista (usada nos resultados das consultas).
void central_mostrar_lista(TabelaHash *tabela, int *ids, int quantidade);

#endif
