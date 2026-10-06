#ifndef CENTRAL_H
#define CENTRAL_H

#include "hash.h"
#include "indices.h"

// Módulo 1 – Central de Ocorrências: cadastrar, consultar, alterar, remover e listar.
// proximo_id é o id que a próxima ocorrência cadastrada vai receber.
void central_menu(TabelaHash *tabela, Indices *indices, int *proximo_id);

// Mostra todos os campos de uma ocorrência
void central_mostrar_ocorrencia(Ocorrencia *o);

// Mostra uma linha resumida para cada id da lista (usada nos resultados das consultas).
void central_mostrar_lista(TabelaHash *tabela, int *ids, int quantidade);

#endif
