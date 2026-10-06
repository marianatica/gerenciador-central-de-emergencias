#ifndef HASH_H
#define HASH_H

#include "ocorrencia.h"

// Menor número primo que deixa o fator de carga abaixo de 0,7 com as 150 ocorrências:
// 150 / 223 = 0,67 

#define TAM_TABELA 223

typedef struct No {
    Ocorrencia ocorrencia;
    struct No *proximo;
} No;

typedef struct {
    No *posicoes[TAM_TABELA]; // cada posição aponta para o começo de uma lista
    int quantidade;           // quantas ocorrências estão guardadas
} TabelaHash;

void hash_inicializar(TabelaHash *tabela);
int hash_inserir(TabelaHash *tabela, Ocorrencia ocorrencia);
Ocorrencia *hash_buscar(TabelaHash *tabela, int id);
int hash_remover(TabelaHash *tabela, int id);
void hash_listar(TabelaHash *tabela);
void hash_liberar(TabelaHash *tabela);

#endif
