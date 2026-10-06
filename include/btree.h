#ifndef BTREE_H
#define BTREE_H

// Arvore B+: todos os dados ficam nas folhas, e as folhas sao ligadas em sequencia

// Ordem m da arvore: cada no guarda no maximo m chaves (e um no interno, no maximo m + 1 filhos).
// Quando um no chega a m + 1 chaves, ele se divide em dois.
#define ORDEM 4
#define TAM_CHAVE  100  // tamanho da maior chave guardada (o endereco)

// Um no da arvore. As chaves ficam sempre em ordem.
// No interno: so orienta o caminho (as chaves sao "placas"); tem quantidade + 1 filhos.
// Folha: guarda os dados de verdade (chave + id da ocorrencia) e aponta para a folha seguinte.
typedef struct NoBMais {
    int folha;                              // 1 = folha, 0 = no interno
    int quantidade;                         // quantas chaves o no tem agora
    char chaves[ORDEM + 1][TAM_CHAVE];      // +1: espaco para a chave extra, logo antes de dividir
    int ids[ORDEM + 1];                     // id da ocorrencia de cada chave
    struct NoBMais *filhos[ORDEM + 2];      // so nos nos internos
    struct NoBMais *proxima;                // so nas folhas: a folha seguinte, em ordem
} NoBMais;

typedef struct {
    NoBMais *raiz;
    int quantidade;     // quantas entradas (chave, id) a arvore guarda
    int nos_visitados;  // quantos nos a ultima busca percorreu
} ArvoreBMais;

void btree_inicializar(ArvoreBMais *arvore);
void btree_inserir(ArvoreBMais *arvore, char *chave, int id);
int btree_buscar_faixa(ArvoreBMais *arvore, char *inicio, char *fim, int *ids, int max);
int btree_buscar_prefixo(ArvoreBMais *arvore, char *prefixo, int *ids, int max);
int btree_remover(ArvoreBMais *arvore, char *chave, int id);
void btree_liberar(ArvoreBMais *arvore);

#endif
