#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "btree.h"

// ---------- Funcoes de apoio ----------

// Compara (chave1, id1) com (chave2, id2): primeiro pelo texto; se o texto for igual, pelo id.
// Retorna um numero negativo se o primeiro vem antes, 0 se sao iguais e positivo se vem depois.
// O id desempata as chaves repetidas (varias ocorrencias com a mesma regiao, por exemplo).
static int comparar(char *chave1, int id1, char *chave2, int id2) {
    int resultado = strcmp(chave1, chave2);
    if (resultado != 0) {
        return resultado;
    }
    return id1 - id2;
}

// Copia uma chave, cortando no tamanho maximo e sempre terminando com '\0'.
static void copiar_chave(char *destino, char *origem) {
    int i = 0;
    while (origem[i] != '\0' && i < TAM_CHAVE - 1) {
        destino[i] = origem[i];
        i++;
    }
    destino[i] = '\0';
}

static NoBMais *criar_no(int folha) {
    NoBMais *no = malloc(sizeof(NoBMais));
    if (no == NULL) {
        printf("Erro: memoria insuficiente.\n");
        exit(1);
    }
    no->folha = folha;
    no->quantidade = 0;
    no->proxima = NULL;
    for (int i = 0; i < ORDEM + 2; i++) {
        no->filhos[i] = NULL;
    }
    return no;
}

// Num no interno, diz para qual filho descer: conta quantas placas sao menores ou iguais a (chave, id).
static int posicao_filho(NoBMais *no, char *chave, int id) {
    int i = 0;
    while (i < no->quantidade && comparar(chave, id, no->chaves[i], no->ids[i]) >= 0) {
        i++;
    }
    return i;
}

// ---------- Inicializar ----------

void btree_inicializar(ArvoreBMais *arvore) {
    arvore->raiz = NULL;
    arvore->quantidade = 0;
    arvore->nos_visitados = 0;
}

// ---------- Inserir ----------

// Divide uma folha que chegou a m + 1 chaves (com m = 4: 5 chaves): a esquerda fica com 2 e a nova folha da direita com 3.
// Na B+, a chave que sobe para o pai e uma COPIA da primeira chave da direita
// (o dado continua na folha, porque todos os dados ficam nas folhas).
static void dividir_folha(NoBMais *no, char *chave_sobe, int *id_sobe, NoBMais **novo) {
    NoBMais *direita = criar_no(1);
    int meio = (ORDEM + 1) / 2;

    for (int j = meio; j < no->quantidade; j++) {
        copiar_chave(direita->chaves[j - meio], no->chaves[j]);
        direita->ids[j - meio] = no->ids[j];
    }
    direita->quantidade = no->quantidade - meio;
    no->quantidade = meio;

    // liga a nova folha na sequencia: esquerda -> direita -> (a que vinha depois)
    direita->proxima = no->proxima;
    no->proxima = direita;

    copiar_chave(chave_sobe, direita->chaves[0]);
    *id_sobe = direita->ids[0];
    *novo = direita;
}

// Divide um no interno que chegou a m + 1 chaves (com m = 4: 5 chaves e 6 filhos): a chave do meio SOBE para o pai,
// saindo deste no; a esquerda fica com 2 chaves (3 filhos) e a nova da direita com 2 chaves (3 filhos).
static void dividir_interno(NoBMais *no, char *chave_sobe, int *id_sobe, NoBMais **novo) {
    NoBMais *direita = criar_no(0);
    int meio = (ORDEM + 1) / 2;
    int k = 0;

    copiar_chave(chave_sobe, no->chaves[meio]);
    *id_sobe = no->ids[meio];

    for (int j = meio + 1; j < no->quantidade; j++) {
        copiar_chave(direita->chaves[k], no->chaves[j]);
        direita->ids[k] = no->ids[j];
        direita->filhos[k] = no->filhos[j];
        k++;
    }
    direita->filhos[k] = no->filhos[no->quantidade];
    direita->quantidade = k;
    no->quantidade = meio;
    *novo = direita;
}

// Insere (chave, id) a partir deste no (recursivo: desce ate a folha certa).
// Se o no estourar, ele se divide: a funcao retorna 1 e preenche chave_sobe/id_sobe
// (a chave que sobe para o pai) e novo (o no criado a direita). Se nao estourar, retorna 0.
static int inserir_no(NoBMais *no, char *chave, int id, char *chave_sobe, int *id_sobe, NoBMais **novo) {
    if (no->folha) {
        // Abre espaco empurrando para a direita as chaves maiores que a nova (como no insertion sort).
        int pos = no->quantidade;
        while (pos > 0 && comparar(chave, id, no->chaves[pos - 1], no->ids[pos - 1]) < 0) {
            copiar_chave(no->chaves[pos], no->chaves[pos - 1]);
            no->ids[pos] = no->ids[pos - 1];
            pos--;
        }
        copiar_chave(no->chaves[pos], chave);
        no->ids[pos] = id;
        no->quantidade++;

        if (no->quantidade <= ORDEM) {
            return 0;
        }
        dividir_folha(no, chave_sobe, id_sobe, novo);
        return 1;
    }

    // No interno: desce para o filho certo.
    int i = posicao_filho(no, chave, id);
    char chave_filho[TAM_CHAVE];
    int id_filho;
    NoBMais *novo_filho;

    if (!inserir_no(no->filhos[i], chave, id, chave_filho, &id_filho, &novo_filho)) {
        return 0; // o filho nao se dividiu: nada muda aqui
    }

    // O filho se dividiu: a chave que subiu entra neste no na posicao i,
    // e o novo filho fica logo a direita dela.
    for (int j = no->quantidade; j > i; j--) {
        copiar_chave(no->chaves[j], no->chaves[j - 1]);
        no->ids[j] = no->ids[j - 1];
        no->filhos[j + 1] = no->filhos[j];
    }
    copiar_chave(no->chaves[i], chave_filho);
    no->ids[i] = id_filho;
    no->filhos[i + 1] = novo_filho;
    no->quantidade++;

    if (no->quantidade <= ORDEM) {
        return 0;
    }
    dividir_interno(no, chave_sobe, id_sobe, novo);
    return 1;
}

void btree_inserir(ArvoreBMais *arvore, char *chave, int id) {
    if (arvore->raiz == NULL) {
        arvore->raiz = criar_no(1); // arvore vazia: a raiz comeca como uma folha
    }

    char chave_sobe[TAM_CHAVE];
    int id_sobe;
    NoBMais *novo;

    if (inserir_no(arvore->raiz, chave, id, chave_sobe, &id_sobe, &novo)) {
        // A raiz se dividiu: cria uma raiz nova acima das duas metades.
        // E so assim que a arvore fica mais alta, por isso todas as folhas continuam na mesma altura.
        NoBMais *nova_raiz = criar_no(0);
        copiar_chave(nova_raiz->chaves[0], chave_sobe);
        nova_raiz->ids[0] = id_sobe;
        nova_raiz->filhos[0] = arvore->raiz;
        nova_raiz->filhos[1] = novo;
        nova_raiz->quantidade = 1;
        arvore->raiz = nova_raiz;
    }
    arvore->quantidade++;
}

// ---------- Buscar ----------

// Desce pelas placas ate a folha onde (chave, id) esta ou deveria estar, contando os nos visitados.
static NoBMais *descer_ate_folha(ArvoreBMais *arvore, char *chave, int id) {
    NoBMais *no = arvore->raiz;
    arvore->nos_visitados = 0;

    while (no != NULL && !no->folha) {
        arvore->nos_visitados++;
        no = no->filhos[posicao_filho(no, chave, id)];
    }
    if (no != NULL) {
        arvore->nos_visitados++;
    }
    return no;
}

// Guarda em ids os ids das chaves entre inicio e fim (inclusive) e retorna quantos achou (no maximo max).
// Para buscar um valor exato, basta usar inicio = fim.
int btree_buscar_faixa(ArvoreBMais *arvore, char *inicio, char *fim, int *ids, int max) {
    int total = 0;
    // id 0 vem antes de qualquer id real, entao cai na primeira folha que pode ter "inicio"
    NoBMais *folha = descer_ate_folha(arvore, inicio, 0);

    while (folha != NULL) {
        for (int i = 0; i < folha->quantidade; i++) {
            if (strcmp(folha->chaves[i], fim) > 0) {
                return total; // passou do fim: como esta tudo em ordem, nao ha mais nada
            }
            if (strcmp(folha->chaves[i], inicio) >= 0 && total < max) {
                ids[total] = folha->ids[i];
                total++;
            }
        }
        folha = folha->proxima; // anda para a folha seguinte, sem voltar para cima na arvore
        if (folha != NULL) {
            arvore->nos_visitados++;
        }
    }
    return total;
}

// Guarda em ids os ids das chaves que comecam com o prefixo e retorna quantos achou (no maximo max).
int btree_buscar_prefixo(ArvoreBMais *arvore, char *prefixo, int *ids, int max) {
    int tamanho = strlen(prefixo);
    int total = 0;
    NoBMais *folha = descer_ate_folha(arvore, prefixo, 0);

    while (folha != NULL) {
        for (int i = 0; i < folha->quantidade; i++) {
            int resultado = strncmp(folha->chaves[i], prefixo, tamanho);
            if (resultado > 0) {
                return total; // ja passou de todas as chaves que comecam com o prefixo
            }
            if (resultado == 0 && total < max) {
                ids[total] = folha->ids[i];
                total++;
            }
        }
        folha = folha->proxima;
        if (folha != NULL) {
            arvore->nos_visitados++;
        }
    }
    return total;
}

// ---------- Remover ----------

// Remove a entrada (chave, id) da folha onde ela esta, SEM rebalancear a arvore:
// a folha pode ficar com menos chaves (ou vazia), mas as buscas continuam corretas,
// porque as placas dos nos internos continuam indicando o caminho certo.
// Retorna 1 se removeu ou 0 se nao encontrou.
int btree_remover(ArvoreBMais *arvore, char *chave, int id) {
    NoBMais *folha = descer_ate_folha(arvore, chave, id);
    if (folha == NULL) {
        return 0;
    }

    for (int i = 0; i < folha->quantidade; i++) {
        if (comparar(chave, id, folha->chaves[i], folha->ids[i]) == 0) {
            // puxa para a esquerda as chaves que vinham depois, tapando o buraco
            for (int j = i; j < folha->quantidade - 1; j++) {
                copiar_chave(folha->chaves[j], folha->chaves[j + 1]);
                folha->ids[j] = folha->ids[j + 1];
            }
            folha->quantidade--;
            arvore->quantidade--;
            return 1;
        }
    }
    return 0;
}

// ---------- Liberar ----------

static void liberar_no(NoBMais *no) {
    if (no == NULL) {
        return;
    }
    if (!no->folha) {
        for (int i = 0; i <= no->quantidade; i++) {
            liberar_no(no->filhos[i]);
        }
    }
    free(no);
}

void btree_liberar(ArvoreBMais *arvore) {
    liberar_no(arvore->raiz);
    arvore->raiz = NULL;
    arvore->quantidade = 0;
}
