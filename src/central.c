#include <stdio.h>
#include <string.h>

#include "central.h"
#include "entrada.h"

void central_mostrar_ocorrencia(Ocorrencia *o) {
    printf("Id:        %d\n", o->id);
    printf("Tipo:      %s\n", o->tipo);
    printf("Descricao: %s\n", o->descricao);
    printf("Regiao:    %s\n", o->regiao);
    printf("Endereco:  %s\n", o->endereco);
    if (o->cep[0] == '\0') {
        printf("CEP:       (vazio)\n");
    } else {
        printf("CEP:       %s\n", o->cep);
    }
    printf("Data/hora: %s\n", o->data_hora);
    printf("Local:     %.6f, %.6f\n", o->lat, o->lng);
}

void central_mostrar_lista(TabelaHash *tabela, int *ids, int quantidade) {
    for (int i = 0; i < quantidade; i++) {
        Ocorrencia *o = hash_buscar(tabela, ids[i]);
        if (o != NULL) {
            printf("%4d | %s | %-30s | %s\n", o->id, o->data_hora, o->tipo, o->regiao);
        }
    }
}

// Confere o formato "AAAA-MM-DD hh:mm:ss": separadores no lugar certo e digitos no resto.
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

// Le um id e recusa valores menores ou iguais a 0 (a funcao hash nao aceita id negativo).
// Retorna o id, ou 0 se for invalido.
static int ler_id(void) {
    int id = ler_inteiro("Id da ocorrencia: ");
    if (id <= 0) {
        printf("Id invalido: precisa ser maior que 0.\n");
        return 0;
    }
    return id;
}

static void cadastrar(TabelaHash *tabela, Indices *indices, int *proximo_id) {
    Ocorrencia nova;

    printf("\n--- Cadastrar ocorrencia ---\n");
    nova.id = *proximo_id;
    ler_texto("Tipo (ex.: EMS: FALL VICTIM): ", nova.tipo, TAM_TIPO);
    if (nova.tipo[0] == '\0') {
        printf("O tipo e obrigatorio. Cadastro cancelado.\n");
        return;
    }
    ler_texto("Descricao: ", nova.descricao, TAM_DESCRICAO);
    ler_texto("Regiao: ", nova.regiao, TAM_REGIAO);
    ler_texto("Endereco: ", nova.endereco, TAM_ENDERECO);
    ler_texto("CEP (pode ficar vazio): ", nova.cep, TAM_CEP);
    ler_texto("Data e hora (AAAA-MM-DD hh:mm:ss): ", nova.data_hora, TAM_DATA);
    if (!data_valida(nova.data_hora)) {
        printf("Data invalida: use o formato AAAA-MM-DD hh:mm:ss. Cadastro cancelado.\n");
        return;
    }
    nova.lat = ler_decimal("Latitude (ex.: 40.12): ");
    nova.lng = ler_decimal("Longitude (ex.: -75.34): ");

    if (hash_inserir(tabela, nova)) {
        indices_adicionar(indices, &nova); // a nova ocorrencia tambem entra nas arvores B+
        printf("Ocorrencia cadastrada com o id %d.\n", nova.id);
        *proximo_id = *proximo_id + 1;
    } else {
        printf("Nao foi possivel cadastrar a ocorrencia.\n");
    }
}

static void consultar(TabelaHash *tabela) {
    printf("\n--- Consultar ocorrencia ---\n");
    int id = ler_id();
    if (id == 0) {
        return;
    }

    Ocorrencia *o = hash_buscar(tabela, id);
    if (o == NULL) {
        printf("Ocorrencia %d nao encontrada.\n", id);
        return;
    }
    central_mostrar_ocorrencia(o);
    printf("(a tabela hash precisou olhar %d ocorrencia(s) para encontrar)\n", tabela->comparacoes);
}

static void alterar(TabelaHash *tabela, Indices *indices) {
    printf("\n--- Alterar ocorrencia ---\n");
    int id = ler_id();
    if (id == 0) {
        return;
    }

    Ocorrencia *o = hash_buscar(tabela, id);
    if (o == NULL) {
        printf("Ocorrencia %d nao encontrada.\n", id);
        return;
    }
    central_mostrar_ocorrencia(o);
    Ocorrencia antiga = *o; // guarda os valores de antes, para tirar das arvores B+

    printf("\nQual campo deseja alterar?\n");
    printf("1 - Tipo\n2 - Descricao\n3 - Regiao\n4 - Endereco\n5 - CEP\n6 - Data/hora\n0 - Cancelar\n");
    int campo = ler_inteiro("Opcao: ");

    if (campo == 1) {
        ler_texto("Novo tipo: ", o->tipo, TAM_TIPO);
    } else if (campo == 2) {
        ler_texto("Nova descricao: ", o->descricao, TAM_DESCRICAO);
    } else if (campo == 3) {
        ler_texto("Nova regiao: ", o->regiao, TAM_REGIAO);
    } else if (campo == 4) {
        ler_texto("Novo endereco: ", o->endereco, TAM_ENDERECO);
    } else if (campo == 5) {
        ler_texto("Novo CEP: ", o->cep, TAM_CEP);
    } else if (campo == 6) {
        char data[TAM_DATA];
        ler_texto("Nova data e hora (AAAA-MM-DD hh:mm:ss): ", data, TAM_DATA);
        if (!data_valida(data)) {
            printf("Data invalida. Nada foi alterado.\n");
            return;
        }
        strcpy(o->data_hora, data);
    } else {
        printf("Alteracao cancelada.\n");
        return;
    }

    // As arvores B+ trocam o valor antigo pelo novo, para as consultas nao acharem o valor velho.
    indices_remover(indices, &antiga);
    indices_adicionar(indices, o);

    // Alteracao autorizada: a hash guarda a nova assinatura do conteudo e aumenta a versao.
    hash_registrar_alteracao(tabela, id);
    printf("Ocorrencia %d alterada.\n", id);
}

static void remover(TabelaHash *tabela, Indices *indices) {
    printf("\n--- Remover ocorrencia ---\n");
    int id = ler_id();
    if (id == 0) {
        return;
    }

    Ocorrencia *o = hash_buscar(tabela, id);
    if (o == NULL) {
        printf("Ocorrencia %d nao encontrada.\n", id);
        return;
    }
    central_mostrar_ocorrencia(o);

    int confirma = ler_inteiro("Digite 1 para confirmar a remocao (0 para cancelar): ");
    if (confirma != 1) {
        printf("Remocao cancelada.\n");
        return;
    }
    indices_remover(indices, o); // tira das arvores B+ antes de apagar da hash
    hash_remover(tabela, id);
    printf("Ocorrencia %d removida.\n", id);
}

void central_menu(TabelaHash *tabela, Indices *indices, int *proximo_id) {
    int opcao = -1;

    while (opcao != 0) {
        printf("\n=== MODULO 1: CENTRAL DE OCORRENCIAS ===\n");
        printf("1 - Cadastrar ocorrencia\n");
        printf("2 - Consultar ocorrencia\n");
        printf("3 - Alterar ocorrencia\n");
        printf("4 - Remover ocorrencia\n");
        printf("5 - Listar ocorrencias\n");
        printf("0 - Voltar\n");
        opcao = ler_inteiro("Opcao: ");

        if (opcao == 1) {
            cadastrar(tabela, indices, proximo_id);
        } else if (opcao == 2) {
            consultar(tabela);
        } else if (opcao == 3) {
            alterar(tabela, indices);
        } else if (opcao == 4) {
            remover(tabela, indices);
        } else if (opcao == 5) {
            printf("\n--- Ocorrencias ---\n");
            hash_listar(tabela);
        } else if (opcao != 0) {
            printf("Opcao invalida.\n");
        }
    }
}
