#include <stdio.h>
#include <string.h>

#include "integridade.h"
#include "central.h"
#include "entrada.h"
#include "csv.h"
#include "btree.h"

#define MAX_OCORRENCIAS 1000

// Area do condado de Montgomery (Pensilvania, EUA), de onde vem todas as chamadas do dataset.
#define LAT_MIN 39.9
#define LAT_MAX 40.6
#define LNG_MIN -75.8
#define LNG_MAX -74.9

// 1) Verifica uma ocorrencia: recalcula a assinatura e compara com a guardada na insercao.
static void verificar_uma(TabelaHash *tabela) {
    int id = ler_inteiro("Id da ocorrencia: ");
    if (id <= 0) {
        printf("Id invalido: precisa ser maior que 0.\n");
        return;
    }
    int resultado = hash_foi_alterada(tabela, id);
    if (resultado == -1) {
        printf("Ocorrencia %d nao encontrada.\n", id);
    } else if (resultado == 1) {
        printf("ATENCAO: a ocorrencia %d foi alterada sem passar pelo sistema (a assinatura nao bate).\n", id);
    } else {
        printf("Ocorrencia %d integra: o conteudo bate com a assinatura registrada.\n", id);
    }
}

// 2) Verifica todas as ocorrencias.
static void verificar_todas(TabelaHash *tabela) {
    int ids[MAX_OCORRENCIAS];
    int n = hash_todos_ids(tabela, ids, MAX_OCORRENCIAS);
    int alteradas = 0;

    for (int i = 0; i < n; i++) {
        if (hash_foi_alterada(tabela, ids[i]) == 1) {
            printf("Alterada sem registro: ");
            central_mostrar_lista(tabela, &ids[i], 1);
            alteradas++;
        }
    }
    printf("%d ocorrencia(s) verificada(s): %d alterada(s) sem registro.\n", n, alteradas);
}

// 3) Registros inconsistentes: campos vazios ou coordenadas fora da area do condado.
static void inconsistentes(TabelaHash *tabela) {
    int ids[MAX_OCORRENCIAS];
    int n = hash_todos_ids(tabela, ids, MAX_OCORRENCIAS);
    int total = 0;

    for (int i = 0; i < n; i++) {
        Ocorrencia *o = hash_buscar(tabela, ids[i]);
        int problema = 0;

        if (o->tipo[0] == '\0' || o->regiao[0] == '\0' || o->endereco[0] == '\0' || o->cep[0] == '\0') {
            problema = 1;
        }
        if (o->lat < LAT_MIN || o->lat > LAT_MAX || o->lng < LNG_MIN || o->lng > LNG_MAX) {
            problema = 1;
        }

        if (problema) {
            printf("Id %d:", o->id);
            if (o->tipo[0] == '\0') printf(" tipo vazio;");
            if (o->regiao[0] == '\0') printf(" regiao vazia;");
            if (o->endereco[0] == '\0') printf(" endereco vazio;");
            if (o->cep[0] == '\0') printf(" CEP vazio;");
            if (o->lat < LAT_MIN || o->lat > LAT_MAX || o->lng < LNG_MIN || o->lng > LNG_MAX) {
                printf(" coordenadas fora do condado;");
            }
            printf("\n");
            total++;
        }
    }
    printf("%d registro(s) inconsistente(s) em %d.\n", total, n);
}

// Monta a chave "data endereco" de uma ocorrencia, cortando no tamanho maximo da chave.
static void montar_chave_evento(char *chave, Ocorrencia *o) {
    int i = 0;
    for (int j = 0; o->data_hora[j] != '\0' && i < TAM_CHAVE - 1; j++) {
        chave[i] = o->data_hora[j];
        i++;
    }
    if (i < TAM_CHAVE - 1) {
        chave[i] = ' ';
        i++;
    }
    for (int j = 0; o->endereco[j] != '\0' && i < TAM_CHAVE - 1; j++) {
        chave[i] = o->endereco[j];
        i++;
    }
    chave[i] = '\0';
}

// 4) Duplicadas e conflitos de versao.
// "Mesmo evento" = mesma data/hora e mesmo endereco. As ocorrencias entram numa Arvore B+
// temporaria com a chave "data endereco"; como a arvore guarda tudo em ordem, as do mesmo evento
// ficam lado a lado nas folhas, e basta comparar cada uma com a seguinte (sem comparar todos os pares).
// Mesmo evento + mesma assinatura = duplicada; mesmo evento + assinatura diferente = conflito de versoes.
static void duplicadas_e_conflitos(TabelaHash *tabela) {
    int ids[MAX_OCORRENCIAS];
    int ordem[MAX_OCORRENCIAS];
    char chave[TAM_CHAVE];
    ArvoreBMais arvore;

    btree_inicializar(&arvore);
    int n = hash_todos_ids(tabela, ids, MAX_OCORRENCIAS);
    for (int i = 0; i < n; i++) {
        Ocorrencia *o = hash_buscar(tabela, ids[i]);
        montar_chave_evento(chave, o);
        btree_inserir(&arvore, chave, o->id);
    }

    int k = btree_buscar_prefixo(&arvore, "", ordem, MAX_OCORRENCIAS); // todas, em ordem
    int duplicadas = 0;
    int conflitos = 0;

    for (int i = 1; i < k; i++) {
        Ocorrencia *a = hash_buscar(tabela, ordem[i - 1]);
        Ocorrencia *b = hash_buscar(tabela, ordem[i]);
        if (strcmp(a->data_hora, b->data_hora) == 0 && strcmp(a->endereco, b->endereco) == 0) {
            if (hash_calcular_assinatura(a) == hash_calcular_assinatura(b)) {
                printf("Duplicada: as ocorrencias %d e %d tem o mesmo conteudo.\n", a->id, b->id);
                duplicadas++;
            } else {
                printf("Conflito de versoes: %d e %d sao o mesmo evento (data e endereco) com conteudos diferentes.\n",
                       a->id, b->id);
                conflitos++;
            }
        }
    }
    printf("%d duplicada(s) e %d conflito(s) de versao.\n", duplicadas, conflitos);
    btree_liberar(&arvore);
}

// 5) Compara o que esta na memoria com o que foi carregado do arquivo CSV.
static void comparar_com_arquivo(TabelaHash *tabela, char *arquivo) {
    static Ocorrencia do_arquivo[MAX_OCORRENCIAS];
    int n = csv_carregar(arquivo, do_arquivo, MAX_OCORRENCIAS);
    if (n == -1) {
        printf("Erro: nao foi possivel abrir o arquivo %s\n", arquivo);
        return;
    }

    int iguais = 0;
    int diferentes = 0;
    int removidas = 0;

    for (int i = 0; i < n; i++) {
        Ocorrencia *o = hash_buscar(tabela, do_arquivo[i].id);
        if (o == NULL) {
            printf("Id %d: esta no arquivo, mas nao esta mais no sistema (foi removida).\n", do_arquivo[i].id);
            removidas++;
        } else if (hash_calcular_assinatura(o) != hash_calcular_assinatura(&do_arquivo[i])) {
            printf("Id %d: o conteudo no sistema e diferente do arquivo.\n", o->id);
            diferentes++;
        } else {
            iguais++;
        }
    }
    printf("Arquivo: %d ocorrencia(s) | iguais: %d | diferentes: %d | removidas: %d | cadastradas depois: %d\n",
           n, iguais, diferentes, removidas, tabela->quantidade - iguais - diferentes);
}

// 6) Simula uma alteracao nao autorizada: muda a regiao direto na memoria, sem registrar
// a nova assinatura (e sem atualizar os indices), para demonstrar que o sistema detecta.
static void simular_alteracao(TabelaHash *tabela) {
    int id = ler_inteiro("Id da ocorrencia: ");
    if (id <= 0) {
        printf("Id invalido: precisa ser maior que 0.\n");
        return;
    }
    Ocorrencia *o = hash_buscar(tabela, id);
    if (o == NULL) {
        printf("Ocorrencia %d nao encontrada.\n", id);
        return;
    }
    ler_texto("Nova regiao (alteracao sem registro): ", o->regiao, TAM_REGIAO);
    printf("Regiao alterada sem passar pelo sistema. Use a opcao 1 ou 2 para detectar.\n");
}

void integridade_menu(TabelaHash *tabela, char *arquivo) {
    int opcao = -1;

    while (opcao != 0) {
        printf("\n=== MODULO 4: INVESTIGACAO (INTEGRIDADE DOS REGISTROS) ===\n");
        printf("1 - Verificar se uma ocorrencia foi alterada\n");
        printf("2 - Verificar todas as ocorrencias\n");
        printf("3 - Registros inconsistentes\n");
        printf("4 - Duplicadas e conflitos de versao\n");
        printf("5 - Comparar com o arquivo carregado\n");
        printf("6 - Simular alteracao nao autorizada (demonstracao)\n");
        printf("0 - Voltar\n");
        opcao = ler_inteiro("Opcao: ");

        if (opcao == 1) {
            verificar_uma(tabela);
        } else if (opcao == 2) {
            verificar_todas(tabela);
        } else if (opcao == 3) {
            inconsistentes(tabela);
        } else if (opcao == 4) {
            duplicadas_e_conflitos(tabela);
        } else if (opcao == 5) {
            comparar_com_arquivo(tabela, arquivo);
        } else if (opcao == 6) {
            simular_alteracao(tabela);
        } else if (opcao != 0) {
            printf("Opcao invalida.\n");
        }
    }
}
