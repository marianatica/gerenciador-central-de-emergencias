#include <stdio.h>
#include <string.h>

#include "central.h"
#include "entrada.h"

void central_mostrar_ocorrencia(Ocorrencia *o) {
    printf("Id:        %d\n", o->id);
    printf("Tipo:      %s\n", o->tipo);
    printf("Descrição: %s\n", o->descricao);
    printf("Região:    %s\n", o->regiao);
    printf("Endereço:  %s\n", o->endereco);
    if (o->cep[0] == '\0') {
        printf("CEP:       (vazio)\n");
    } else {
        printf("CEP:       %s\n", o->cep);
    }
    printf("Data/hora: %s\n", o->data_hora);
    printf("Local:     %.6f, %.6f\n", o->lat, o->lng);
}

// Confere o formato "AAAA-MM-DD hh:mm:ss": separadores no lugar certo e dígitos no resto.
static int data_valida(char *data) {
    if (strlen(data) != TAM_DATA - 1) {
        return 0;
    }
    for (int i = 0; i < TAM_DATA - 1; i++) {
        if (i == 4 || i == 7) {
            if (data[i] != '-') {
                return 0;
            }
        } else if (i == 10) {
            if (data[i] != ' ') {
                return 0;
            }
        } else if (i == 13 || i == 16) {
            if (data[i] != ':') {
                return 0;
            }
        } else if (data[i] < '0' || data[i] > '9') {
            return 0;
        }
    }
    return 1;
}

// Lê um id e recusa valores menores ou iguais a 0 (a função hash não aceita id negativo).
// Retorna o id, ou 0 se for inválido.
static int ler_id(void) {
    int id = ler_inteiro("Id da ocorrência: ");
    if (id <= 0) {
        printf("Id inválido: precisa ser maior que 0.\n");
        return 0;
    }
    return id;
}

static void cadastrar(TabelaHash *tabela, int *proximo_id) {
    Ocorrencia nova;

    printf("\n--- Cadastrar ocorrência ---\n");
    nova.id = *proximo_id;
    ler_texto("Tipo (ex.: EMS: FALL VICTIM): ", nova.tipo, TAM_TIPO);
    if (nova.tipo[0] == '\0') {
        printf("O tipo é obrigatório. Cadastro cancelado.\n");
        return;
    }
    ler_texto("Descrição: ", nova.descricao, TAM_DESCRICAO);
    ler_texto("Região: ", nova.regiao, TAM_REGIAO);
    ler_texto("Endereço: ", nova.endereco, TAM_ENDERECO);
    ler_texto("CEP (pode ficar vazio): ", nova.cep, TAM_CEP);
    ler_texto("Data e hora (AAAA-MM-DD hh:mm:ss): ", nova.data_hora, TAM_DATA);
    if (!data_valida(nova.data_hora)) {
        printf("Data inválida: use o formato AAAA-MM-DD hh:mm:ss. Cadastro cancelado.\n");
        return;
    }
    nova.lat = ler_decimal("Latitude (ex.: 40.12): ");
    nova.lng = ler_decimal("Longitude (ex.: -75.34): ");

    if (hash_inserir(tabela, nova)) {
        printf("Ocorrência cadastrada com o id %d.\n", nova.id);
        *proximo_id = *proximo_id + 1;
    } else {
        printf("Não foi possível cadastrar a ocorrência.\n");
    }
}

static void consultar(TabelaHash *tabela) {
    printf("\n--- Consultar ocorrência ---\n");
    int id = ler_id();
    if (id == 0) {
        return;
    }

    Ocorrencia *o = hash_buscar(tabela, id);
    if (o == NULL) {
        printf("Ocorrência %d não encontrada.\n", id);
        return;
    }
    central_mostrar_ocorrencia(o);
    printf("(a tabela hash precisou olhar %d ocorrência(s) para encontrar)\n", tabela->comparacoes);
}

static void alterar(TabelaHash *tabela) {
    printf("\n--- Alterar ocorrência ---\n");
    int id = ler_id();
    if (id == 0) {
        return;
    }

    Ocorrencia *o = hash_buscar(tabela, id);
    if (o == NULL) {
        printf("Ocorrência %d não encontrada.\n", id);
        return;
    }
    central_mostrar_ocorrencia(o);

    printf("\nQual campo deseja alterar?\n");
    printf("1 - Tipo\n2 - Descrição\n3 - Região\n4 - Endereço\n5 - CEP\n6 - Data/hora\n0 - Cancelar\n");
    int campo = ler_inteiro("Opção: ");

    if (campo == 1) {
        ler_texto("Novo tipo: ", o->tipo, TAM_TIPO);
    } else if (campo == 2) {
        ler_texto("Nova descrição: ", o->descricao, TAM_DESCRICAO);
    } else if (campo == 3) {
        ler_texto("Nova região: ", o->regiao, TAM_REGIAO);
    } else if (campo == 4) {
        ler_texto("Novo endereço: ", o->endereco, TAM_ENDERECO);
    } else if (campo == 5) {
        ler_texto("Novo CEP: ", o->cep, TAM_CEP);
    } else if (campo == 6) {
        char data[TAM_DATA];
        ler_texto("Nova data e hora (AAAA-MM-DD hh:mm:ss): ", data, TAM_DATA);
        if (!data_valida(data)) {
            printf("Data inválida. Nada foi alterado.\n");
            return;
        }
        strcpy(o->data_hora, data);
    } else {
        printf("Alteração cancelada.\n");
        return;
    }

    // Alteração autorizada: a hash guarda a nova assinatura do conteúdo e aumenta a versão.
    hash_registrar_alteracao(tabela, id);
    printf("Ocorrência %d alterada.\n", id);
}

static void remover(TabelaHash *tabela) {
    printf("\n--- Remover ocorrência ---\n");
    int id = ler_id();
    if (id == 0) {
        return;
    }

    Ocorrencia *o = hash_buscar(tabela, id);
    if (o == NULL) {
        printf("Ocorrência %d não encontrada.\n", id);
        return;
    }
    central_mostrar_ocorrencia(o);

    int confirma = ler_inteiro("Digite 1 para confirmar a remoção (0 para cancelar): ");
    if (confirma != 1) {
        printf("Remoção cancelada.\n");
        return;
    }
    hash_remover(tabela, id);
    printf("Ocorrência %d removida.\n", id);
}

void central_menu(TabelaHash *tabela, int *proximo_id) {
    int opcao = -1;

    while (opcao != 0) {
        printf("\n=== MÓDULO 1: CENTRAL DE OCORRÊNCIAS ===\n");
        printf("1 - Cadastrar ocorrência\n");
        printf("2 - Consultar ocorrência\n");
        printf("3 - Alterar ocorrência\n");
        printf("4 - Remover ocorrência\n");
        printf("5 - Listar ocorrências\n");
        printf("0 - Voltar\n");
        opcao = ler_inteiro("Opção: ");

        if (opcao == 1) {
            cadastrar(tabela, proximo_id);
        } else if (opcao == 2) {
            consultar(tabela);
        } else if (opcao == 3) {
            alterar(tabela);
        } else if (opcao == 4) {
            remover(tabela);
        } else if (opcao == 5) {
            printf("\n--- Ocorrências ---\n");
            hash_listar(tabela);
        } else if (opcao != 0) {
            printf("Opção inválida.\n");
        }
    }
}
