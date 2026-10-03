#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "csv.h"

#define TAM_LINHA   1000
#define NUM_COLUNAS 9
#define TAM_CAMPO   200

// Copia o texto para o campo da struct sem passar do tamanho dele.
static void copiar(char *destino, char *origem, int tamanho) {
    int i = 0;
    while (origem[i] != '\0' && i < tamanho - 1) {
        destino[i] = origem[i];
        i++;
    }
    destino[i] = '\0';
}

// Separa a linha nas vírgulas, guardando cada campo em campos[0], campos[1], ...
// Um campo vazio (",,") vira um texto vazio, então os campos seguintes não saem do lugar.
static int separar_campos(char *linha, char campos[][TAM_CAMPO]) {
    int c = 0; // número do campo atual
    int j = 0; // posição dentro do campo atual

    for (int i = 0; linha[i] != '\0' && linha[i] != '\n' && linha[i] != '\r'; i++) {
        if (linha[i] == ',') {
            campos[c][j] = '\0'; // termina o campo atual
            c++;
            j = 0;
            if (c == NUM_COLUNAS) {
                return c + 1; // mais campos que o esperado
            }
        } else if (j < TAM_CAMPO - 1) {
            campos[c][j] = linha[i];
            j++;
        }
    }
    campos[c][j] = '\0';
    return c + 1;
}

int csv_carregar(const char *caminho, Ocorrencia *vetor, int max) {
    FILE *arquivo = fopen(caminho, "r");
    if (arquivo == NULL) {
        return -1;
    }

    char linha[TAM_LINHA];
    char campos[NUM_COLUNAS][TAM_CAMPO];
    int lidas = 0;
    int ignoradas = 0;

    fgets(linha, TAM_LINHA, arquivo); // pula o cabeçalho

    while (lidas < max && fgets(linha, TAM_LINHA, arquivo) != NULL) {
        int n = separar_campos(linha, campos);

        // Colunas do arquivo: lat,lng,desc,zip,title,timeStamp,twp,addr,e
        // A linha precisa ter os 9 campos e a data no formato AAAA-MM-DD hh:mm:ss
        if (n == NUM_COLUNAS && strlen(campos[5]) == TAM_DATA - 1) {
            vetor[lidas].id = lidas + 1;
            vetor[lidas].lat = atof(campos[0]);
            vetor[lidas].lng = atof(campos[1]);
            copiar(vetor[lidas].descricao, campos[2], TAM_DESCRICAO);
            copiar(vetor[lidas].cep, campos[3], TAM_CEP);
            copiar(vetor[lidas].tipo, campos[4], TAM_TIPO);
            copiar(vetor[lidas].data_hora, campos[5], TAM_DATA);
            copiar(vetor[lidas].regiao, campos[6], TAM_REGIAO);
            copiar(vetor[lidas].endereco, campos[7], TAM_ENDERECO);

            // Campos derivados: são calculados depois.
            vetor[lidas].prioridade = 0;
            vetor[lidas].tempo_estimado_min = 0;
            vetor[lidas].pessoas = 0;
            vetor[lidas].status = STATUS_PENDENTE;
            vetor[lidas].equipe[0] = '\0';

            lidas++;
        } else {
            ignoradas++;
        }
    }

    fclose(arquivo);

    if (ignoradas > 0) {
        printf("Aviso: %d linha(s) do CSV ignorada(s) por formato inválido\n", ignoradas);
    }
    return lidas;
}
