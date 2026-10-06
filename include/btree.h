#ifndef BTREE_H
#define BTREE_H

// Árvore B+: todos os dados ficam nas folhas, e as folhas são ligadas em sequência

// Ordem m da árvore: cada nó guarda no máximo m chaves (e um nó interno, no máximo m + 1 filhos).
// Quando um nó chega a m + 1 chaves, ele se divide em dois.
#define ORDEM 4
#define TAM_CHAVE  100  // tamanho da maior chave guardada (o endereço)

// Um nó da árvore. As chaves ficam sempre em ordem.
// Nó interno: só orienta o caminho (as chaves são "placas"); tem quantidade + 1 filhos.
// Folha: guarda os dados de verdade (chave + id da ocorrência) e aponta para a folha seguinte.
typedef struct NoBMais {
    int folha;                              // 1 = folha, 0 = nó interno
    int quantidade;                         // quantas chaves o nó tem agora
    char chaves[ORDEM + 1][TAM_CHAVE];      // +1: espaço para a chave extra, logo antes de dividir
    int ids[ORDEM + 1];                     // id da ocorrência de cada chave
    struct NoBMais *filhos[ORDEM + 2];      // só nos nós internos
    struct NoBMais *proxima;                // só nas folhas: a folha seguinte, em ordem
} NoBMais;

typedef struct {
    NoBMais *raiz;
    int quantidade;     // quantas entradas (chave, id) a árvore guarda
    int nos_visitados;  // quantos nós a última busca percorreu
} ArvoreBMais;

void btree_inicializar(ArvoreBMais *arvore);
void btree_inserir(ArvoreBMais *arvore, char *chave, int id);
int btree_buscar_faixa(ArvoreBMais *arvore, char *inicio, char *fim, int *ids, int max);
int btree_buscar_prefixo(ArvoreBMais *arvore, char *prefixo, int *ids, int max);
int btree_remover(ArvoreBMais *arvore, char *chave, int id);
int btree_altura(ArvoreBMais *arvore);
void btree_liberar(ArvoreBMais *arvore);

#endif
