#include <stdio.h>

// Para compilar:  gcc -Iinclude src/main.c -o central

#include "csv.c"
#include "hash.c"
#include "btree.c"
#include "indices.c"
#include "entrada.c"
#include "central.c"
#include "consultas.c"
#include "integridade.c"
#include "greedy.c"

#ifdef _WIN32
#include <windows.h>
#endif

#define ARQUIVO_DADOS   "data/ocorrencias_911.csv"
#define MAX_OCORRENCIAS 1000

int main(void) {
#ifdef _WIN32
    SetConsoleOutputCP(65001); // faz o terminal do Windows mostrar os acentos
#endif

    // static: o vetor é grande (cerca de 400 KB) e assim fica fora da pilha da função.
    static Ocorrencia lidas[MAX_OCORRENCIAS];
    TabelaHash tabela;
    Indices indices;

    // Carregar os registros iniciais: lê o CSV e guarda cada ocorrência na tabela hash
    // e nas árvores B+ (índices por tipo, região e data).
    int n = csv_carregar(ARQUIVO_DADOS, lidas, MAX_OCORRENCIAS);
    if (n == -1) {
        printf("Erro: não foi possível abrir o arquivo %s\n", ARQUIVO_DADOS);
        return 1;
    }
    hash_inicializar(&tabela);
    indices_inicializar(&indices);
    for (int i = 0; i < n; i++) {
        if (hash_inserir(&tabela, lidas[i])) {
            indices_adicionar(&indices, &lidas[i]);
        }
    }
    printf("%d ocorrências carregadas de %s\n", tabela.quantidade, ARQUIVO_DADOS);

    int proximo_id = n + 1; // os ids do arquivo vão de 1 a n
    int opcao = -1;

    while (opcao != 0) {
        printf("\n=== CENTRAL DE EMERGÊNCIAS: OPERAÇÃO RESGATE ===\n");
        printf("1 - Central de Ocorrências\n");
        printf("2 - Consulta Rápida\n");
        printf("3 - Organização das Ocorrências\n");
        printf("4 - Investigação (Integridade)\n");
        printf("5 - Operação Resgate\n");
        printf("0 - Sair\n");
        opcao = ler_inteiro("Opção: ");

        if (opcao == 1) {
            central_menu(&tabela, &indices, &proximo_id);
        } else if (opcao == 2) {
            consulta_rapida_menu(&tabela, &indices);
        } else if (opcao == 3) {
            organizacao_menu(&tabela, &indices);
        } else if (opcao == 4) {
            integridade_menu(&tabela, ARQUIVO_DADOS);
        } else if (opcao == 5) {
            greedy_menu(&tabela, &indices);
        } else if (opcao != 0) {
            printf("Opção inválida.\n");
        }
    }

    indices_liberar(&indices);
    hash_liberar(&tabela);
    printf("Até logo!\n");
    return 0;
}
