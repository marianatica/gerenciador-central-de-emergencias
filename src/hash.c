#include <stdio.h>
#include <stdlib.h>

#include "hash.h"

static int calcular_posicao(int id) {
    return id % TAM_TABELA;
}

// Deixa todas as posicoes vazias.
void hash_inicializar(TabelaHash *tabela) {
    for (int i = 0; i < TAM_TABELA; i++) {
        tabela->posicoes[i] = NULL;
    }
    tabela->quantidade = 0;
    tabela->comparacoes = 0;
}

// Procura o no que guarda o id e conta quantas ocorrencias precisou olhar.
// Retorna o no, ou NULL se o id nao existe.
static No *buscar_no(TabelaHash *tabela, int id) {
    No *atual = tabela->posicoes[calcular_posicao(id)];
    tabela->comparacoes = 0;

    while (atual != NULL) {
        tabela->comparacoes++;
        if (atual->ocorrencia.id == id) {
            return atual;
        }
        atual = atual->proximo;
    }
    return NULL;
}

// Retorna 1 se inseriu ou 0 se ja existe uma ocorrencia com esse id.
int hash_inserir(TabelaHash *tabela, Ocorrencia ocorrencia) {
    if (hash_buscar(tabela, ocorrencia.id) != NULL) {
        return 0; // o id nao pode se repetir
    }

    No *novo = malloc(sizeof(No));
    if (novo == NULL) {
        return 0;
    }

    int pos = calcular_posicao(ocorrencia.id);
    novo->ocorrencia = ocorrencia;
    novo->assinatura = hash_calcular_assinatura(&novo->ocorrencia); // "lacre" do conteudo no momento da insercao
    novo->versao = 1;
    novo->proximo = tabela->posicoes[pos]; // o novo entra no comeco da lista
    tabela->posicoes[pos] = novo;
    tabela->quantidade++;
    return 1;
}

// Retorna o endereco da ocorrencia dentro da tabela, ou NULL se o id nao existe.
Ocorrencia *hash_buscar(TabelaHash *tabela, int id) {
    No *no = buscar_no(tabela, id);
    if (no == NULL) {
        return NULL;
    }
    return &no->ocorrencia;
}

// Retorna 1 se removeu, ou 0 se o id nao existe.
int hash_remover(TabelaHash *tabela, int id) {
    int pos = calcular_posicao(id);
    No *atual = tabela->posicoes[pos];
    No *anterior = NULL;

    while (atual != NULL) {
        if (atual->ocorrencia.id == id) {
            if (anterior == NULL) {
                tabela->posicoes[pos] = atual->proximo; // era o primeiro da lista
            } else {
                anterior->proximo = atual->proximo; // liga o anterior ao proximo, pulando o removido
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

// Mostra todas as ocorrencias, percorrendo as posicoes da tabela uma a uma.
void hash_listar(TabelaHash *tabela) {
    for (int i = 0; i < TAM_TABELA; i++) {
        No *atual = tabela->posicoes[i];
        while (atual != NULL) {
            Ocorrencia *o = &atual->ocorrencia;
            printf("%4d | %s | %-30s | %s\n", o->id, o->data_hora, o->tipo, o->regiao);
            atual = atual->proximo;
        }
    }
    printf("Total: %d ocorrencia(s)\n", tabela->quantidade);
}

// Guarda em ids os ids de todas as ocorrencias (na ordem das posicoes da tabela) e retorna quantos.
// Usada pelas buscas sequenciais, que precisam olhar todas as ocorrencias uma a uma.
int hash_todos_ids(TabelaHash *tabela, int *ids, int max) {
    int total = 0;
    for (int i = 0; i < TAM_TABELA; i++) {
        No *atual = tabela->posicoes[i];
        while (atual != NULL) {
            if (total < max) {
                ids[total] = atual->ocorrencia.id;
                total++;
            }
            atual = atual->proximo;
        }
    }
    return total;
}

// Libera a memoria de todos os itens da tabela.
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

// ---------- Integridade (Modulo 4) ----------

// Continua a conta da assinatura com as letras de um texto (algoritmo djb2: h = h * 33 + letra).
// O '|' no final separa um campo do outro, para "AB" + "C" nao dar o mesmo que "A" + "BC".
static unsigned long misturar_texto(unsigned long h, char *texto) {
    for (int i = 0; texto[i] != '\0'; i++) {
        h = h * 33 + texto[i];
    }
    return h * 33 + '|';
}

// Assinatura: um numero calculado a partir de todo o conteudo da ocorrencia
unsigned long hash_calcular_assinatura(Ocorrencia *o) {
    unsigned long h = 5381;

    h = misturar_texto(h, o->tipo);
    h = misturar_texto(h, o->descricao);
    h = misturar_texto(h, o->regiao);
    h = misturar_texto(h, o->endereco);
    h = misturar_texto(h, o->cep);
    h = misturar_texto(h, o->data_hora);
    // as coordenadas entram como numeros inteiros (6 casas decimais)
    h = h * 33 + (long)(o->lat * 1000000);
    h = h * 33 + (long)(o->lng * 1000000);
    return h;
}

// Alteracao autorizada feita pelo menu: guarda a nova assinatura e aumenta a versao.
// Retorna 1 se registrou, ou 0 se o id nao existe.
int hash_registrar_alteracao(TabelaHash *tabela, int id) {
    No *no = buscar_no(tabela, id);
    if (no == NULL) {
        return 0;
    }
    no->assinatura = hash_calcular_assinatura(&no->ocorrencia);
    no->versao++;
    return 1;
}

// Compara o conteudo atual com a assinatura guardada.
// Retorna 1 se foi alterada sem registro, 0 se esta integra, ou -1 se o id nao existe.
int hash_foi_alterada(TabelaHash *tabela, int id) {
    No *no = buscar_no(tabela, id);
    if (no == NULL) {
        return -1;
    }
    if (hash_calcular_assinatura(&no->ocorrencia) != no->assinatura) {
        return 1;
    }
    return 0;
}
