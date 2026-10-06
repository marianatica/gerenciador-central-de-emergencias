#ifndef INDICES_H
#define INDICES_H

#include "ocorrencia.h"
#include "btree.h"

// Indices das ocorrencias: uma Arvore B+ para cada campo usado nas consultas.
// Cada arvore guarda (valor do campo, id) em ordem; a ocorrencia completa fica na tabela hash.
typedef struct {
    ArvoreBMais tipo;
    ArvoreBMais regiao;
    ArvoreBMais data;
} Indices;

void indices_inicializar(Indices *indices);
void indices_adicionar(Indices *indices, Ocorrencia *o);
void indices_remover(Indices *indices, Ocorrencia *o);
void indices_liberar(Indices *indices);

#endif
