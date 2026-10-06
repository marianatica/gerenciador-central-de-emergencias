#include <stdio.h>
#include <stdlib.h>

#include "hash.h"

static int calcular_posicao(int id) {
    return id % TAM_TABELA;
}

// Deixa todas as posições vazias.
void hash_inicializar(TabelaHash *tabela) {
    for (int i = 0; i < TAM_TABELA; i++) {
        tabela->posicoes[i] = NULL;
    }
    tabela->quantidade = 0;
}

// Retorna 1 se inseriu ou 0 se já existe uma ocorrência com esse id.
int hash_inserir(TabelaHash *tabela, Ocorrencia ocorrencia) {
    if (hash_buscar(tabela, ocorrencia.id) != NULL) {
        return 0; // o id não pode se repetir
    }

    No *novo = malloc(sizeof(No));
    if (novo == NULL) {
        return 0;
    }

    int pos = calcular_posicao(ocorrencia.id);
    novo->ocorrencia = ocorrencia;
    novo->proximo = tabela->posicoes[pos]; // o novo entra no começo da lista
    tabela->posicoes[pos] = novo;
    tabela->quantidade++;
    return 1;
}

// Retorna o endereço da ocorrência dentro da tabela, ou NULL se o id não existe.
Ocorrencia *hash_buscar(TabelaHash *tabela, int id) {
    No *atual = tabela->posicoes[calcular_posicao(id)];

    while (atual != NULL) {
        if (atual->ocorrencia.id == id) {
            return &atual->ocorrencia;
        }
        atual = atual->proximo;
    }
    return NULL;
}

// Retorna 1 se removeu, ou 0 se o id não existe.
int hash_remover(TabelaHash *tabela, int id) {
    int pos = calcular_posicao(id);
    No *atual = tabela->posicoes[pos];
    No *anterior = NULL;

    while (atual != NULL) {
        if (atual->ocorrencia.id == id) {
            if (anterior == NULL) {
                tabela->posicoes[pos] = atual->proximo; // era o primeiro da lista
            } else {
                anterior->proximo = atual->proximo; // liga o anterior ao próximo, pulando o removido
            }
            free(atual);
            tabela->quantidade--;
            return 1;
        }
        anterior = atual;
        atual = atual->proximo;
    }
    return 0;
}

// Mostra todas as ocorrências, percorrendo as posições da tabela uma a uma.
void hash_listar(TabelaHash *tabela) {
    for (int i = 0; i < TAM_TABELA; i++) {
        No *atual = tabela->posicoes[i];
        while (atual != NULL) {
            Ocorrencia *o = &atual->ocorrencia;
            printf("%4d | %s | %-30s | %s\n", o->id, o->data_hora, o->tipo, o->regiao);
            atual = atual->proximo;
        }
    }
    printf("Total: %d ocorrência(s)\n", tabela->quantidade);
}

// Libera a memória de todos os itens da tabela.
void hash_liberar(TabelaHash *tabela) {
    for (int i = 0; i < TAM_TABELA; i++) {
        No *atual = tabela->posicoes[i];
        while (atual != NULL) {
            No *proximo = atual->proximo;
            free(atual);
            atual = proximo;
        }
        tabela->posicoes[i] = NULL;
    }
    tabela->quantidade = 0;
}
