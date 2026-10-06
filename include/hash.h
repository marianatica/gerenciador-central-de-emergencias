#ifndef HASH_H
#define HASH_H

#include "ocorrencia.h"

// Menor número primo que deixa o fator de carga abaixo de 0,7 com as 150 ocorrências:
// 150 / 223 = 0,67 

#define TAM_TABELA 223

typedef struct No {
    Ocorrencia ocorrencia;
    unsigned long assinatura; // número calculado a partir do conteúdo (Módulo 4: integridade)
    int versao;               // começa em 1 e aumenta a cada alteração feita pelo sistema
    struct No *proximo;
} No;

typedef struct {
    No *posicoes[TAM_TABELA]; // cada posição aponta para o começo de uma lista
    int quantidade;           // quantas ocorrências estão guardadas
    int comparacoes;          // quantas ocorrências a última busca precisou olhar
} TabelaHash;

void hash_inicializar(TabelaHash *tabela);
int hash_inserir(TabelaHash *tabela, Ocorrencia ocorrencia);
Ocorrencia *hash_buscar(TabelaHash *tabela, int id);
int hash_remover(TabelaHash *tabela, int id);
void hash_listar(TabelaHash *tabela);
int hash_todos_ids(TabelaHash *tabela, int *ids, int max);
void hash_liberar(TabelaHash *tabela);

unsigned long hash_calcular_assinatura(Ocorrencia *o);
int hash_registrar_alteracao(TabelaHash *tabela, int id);
int hash_foi_alterada(TabelaHash *tabela, int id);

#endif
