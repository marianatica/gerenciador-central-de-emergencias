#ifndef HASH_H
#define HASH_H

#include "ocorrencia.h"

// Menor numero primo que deixa o fator de carga abaixo de 0,7 com as 150 ocorrencias:
// 150 / 223 = 0,67 

#define TAM_TABELA 223

typedef struct No {
    Ocorrencia ocorrencia;
    unsigned long assinatura; // numero calculado a partir do conteudo (Modulo 4: integridade)
    int versao;               // comeca em 1 e aumenta a cada alteracao feita pelo sistema
    struct No *proximo;
} No;

typedef struct {
    No *posicoes[TAM_TABELA]; // cada posicao aponta para o comeco de uma lista
    int quantidade;           // quantas ocorrencias estao guardadas
    int comparacoes;          // quantas ocorrencias a ultima busca precisou olhar
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
