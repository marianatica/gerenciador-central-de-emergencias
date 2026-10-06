#include <stdio.h>
#include <string.h>

#include "consultas.h"
#include "central.h"
#include "entrada.h"

#define MAX_RESULTADOS 1000

static int resultados[MAX_RESULTADOS]; // ids encontrados na ultima busca

static void mostrar_resultados(TabelaHash *tabela, int quantidade) {
    if (quantidade == 0) {
        printf("Nenhuma ocorrencia encontrada.\n");
        return;
    }
    central_mostrar_lista(tabela, resultados, quantidade);
    printf("%d ocorrencia(s) encontrada(s).\n", quantidade);
}

// Compara quantos nos a Arvore B+ visitou com as ocorrencias que a busca sequencial teria que olhar.
static void mostrar_eficiencia_arvore(TabelaHash *tabela, ArvoreBMais *arvore) {
    printf("Eficiencia: a Arvore B+ visitou %d no(s); a busca sequencial olharia as %d ocorrencias.\n",
           arvore->nos_visitados, tabela->quantidade);
}

// ---------- Buscas ----------

// Tabela Hash: vai direto a posicao do id.
static void por_id(TabelaHash *tabela) {
    int id = ler_inteiro("Id: ");
    if (id <= 0) {
        printf("Id invalido: precisa ser maior que 0.\n");
        return;
    }
    Ocorrencia *o = hash_buscar(tabela, id);
    if (o == NULL) {
        printf("Ocorrencia %d nao encontrada.\n", id);
    } else {
        central_mostrar_ocorrencia(o);
    }
    printf("Eficiencia: a Tabela Hash olhou %d ocorrencia(s); a busca sequencial olharia ate %d.\n",
           tabela->comparacoes, tabela->quantidade);
}

// Busca sequencial: a palavra pode estar no meio da descricao, e nenhum dos indices
// (que sao ordenados pelo comeco do texto) ajuda nesse caso.
static void por_descricao(TabelaHash *tabela) {
    char palavra[TAM_DESCRICAO];
    int todos[MAX_RESULTADOS];

    ler_texto("Palavra da descricao, como esta no dataset (ex.: Station 308A ou NORRISTOWN): ", palavra, TAM_DESCRICAO);
    int n = hash_todos_ids(tabela, todos, MAX_RESULTADOS);
    int achados = 0;
    for (int i = 0; i < n; i++) {
        Ocorrencia *o = hash_buscar(tabela, todos[i]);
        if (strstr(o->descricao, palavra) != NULL) {
            resultados[achados] = todos[i];
            achados++;
        }
    }
    mostrar_resultados(tabela, achados);
    printf("Eficiencia: busca sequencial, olhou as %d ocorrencias.\n", n);
}

// Arvore B+: busca exata = faixa que comeca e termina no mesmo valor.
static void por_valor_exato(TabelaHash *tabela, ArvoreBMais *arvore, char *pergunta) {
    char texto[TAM_CHAVE];
    ler_texto(pergunta, texto, TAM_CHAVE);
    int n = btree_buscar_faixa(arvore, texto, texto, resultados, MAX_RESULTADOS);
    mostrar_resultados(tabela, n);
    mostrar_eficiencia_arvore(tabela, arvore);
}

// Arvore B+: desce ate o primeiro valor que comeca com o prefixo e anda pelas folhas.
static void por_prefixo(TabelaHash *tabela, ArvoreBMais *arvore, char *pergunta) {
    char prefixo[TAM_CHAVE];
    ler_texto(pergunta, prefixo, TAM_CHAVE);
    int n = btree_buscar_prefixo(arvore, prefixo, resultados, MAX_RESULTADOS);
    mostrar_resultados(tabela, n);
    mostrar_eficiencia_arvore(tabela, arvore);
}

// Arvore B+: como a data e texto "AAAA-MM-DD hh:mm:ss", a ordem alfabetica e a ordem do tempo.
static void por_datas(TabelaHash *tabela, Indices *indices) {
    char inicio[TAM_DATA];
    char fim[TAM_DATA];
    ler_texto("Data/hora inicial (AAAA-MM-DD hh:mm:ss): ", inicio, TAM_DATA);
    ler_texto("Data/hora final   (AAAA-MM-DD hh:mm:ss): ", fim, TAM_DATA);
    int n = btree_buscar_faixa(&indices->data, inicio, fim, resultados, MAX_RESULTADOS);
    mostrar_resultados(tabela, n);
    mostrar_eficiencia_arvore(tabela, &indices->data);
}

// Arvore B+: com prefixo vazio, todas as chaves combinam, e as folhas ja estao em ordem.
static void listar_em_ordem(TabelaHash *tabela, ArvoreBMais *arvore) {
    int n = btree_buscar_prefixo(arvore, "", resultados, MAX_RESULTADOS);
    mostrar_resultados(tabela, n);
}

// ---------- Menus ----------

void consulta_rapida_menu(TabelaHash *tabela, Indices *indices) {
    int opcao = -1;

    while (opcao != 0) {
        printf("\n=== MODULO 2: CONSULTA RAPIDA ===\n");
        printf("1 - Por id (Tabela Hash)\n");
        printf("2 - Por palavra da descricao (busca sequencial)\n");
        printf("3 - Por tipo (Arvore B+)\n");
        printf("4 - Por regiao (Arvore B+)\n");
        printf("5 - Por prefixo do tipo (Arvore B+)\n");
        printf("6 - Por intervalo de datas (Arvore B+)\n");
        printf("0 - Voltar\n");
        opcao = ler_inteiro("Opcao: ");

        if (opcao == 1) {
            por_id(tabela);
        } else if (opcao == 2) {
            por_descricao(tabela);
        } else if (opcao == 3) {
            por_valor_exato(tabela, &indices->tipo, "Tipo (ex.: EMS: CARDIAC EMERGENCY): ");
        } else if (opcao == 4) {
            por_valor_exato(tabela, &indices->regiao, "Regiao (ex.: NORRISTOWN): ");
        } else if (opcao == 5) {
            por_prefixo(tabela, &indices->tipo, "Comeco do tipo (ex.: EMS, Fire, Traffic: VEH): ");
        } else if (opcao == 6) {
            por_datas(tabela, indices);
        } else if (opcao != 0) {
            printf("Opcao invalida.\n");
        }
    }
}

void organizacao_menu(TabelaHash *tabela, Indices *indices) {
    int opcao = -1;

    while (opcao != 0) {
        printf("\n=== MODULO 3: ORGANIZACAO DAS OCORRENCIAS ===\n");
        printf("1 - Ocorrencias de uma regiao\n");
        printf("2 - Ocorrencias de uma categoria (EMS, Fire ou Traffic)\n");
        printf("3 - Todas em ordem cronologica\n");
        printf("4 - Todas em ordem de regiao\n");
        printf("0 - Voltar\n");
        opcao = ler_inteiro("Opcao: ");

        if (opcao == 1) {
            por_valor_exato(tabela, &indices->regiao, "Regiao (ex.: NORRISTOWN): ");
        } else if (opcao == 2) {
            por_prefixo(tabela, &indices->tipo, "Categoria (EMS, Fire ou Traffic): ");
        } else if (opcao == 3) {
            listar_em_ordem(tabela, &indices->data);
        } else if (opcao == 4) {
            listar_em_ordem(tabela, &indices->regiao);
        } else if (opcao != 0) {
            printf("Opcao invalida.\n");
        }
    }
}
