#ifndef INDICES_H
#define INDICES_H

#include "ocorrencia.h"
#include "btree.h"

// Índices das ocorrências: uma Árvore B+ para cada campo usado nas consultas.
// Cada árvore guarda (valor do campo, id) em ordem; a ocorrência completa fica na tabela hash.
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
