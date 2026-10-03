#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "csv.h"

#define TAM_LINHA 1000

// Ordem das colunas no arquivo: lat,lng,desc,zip,title,timeStamp,twp,addr,e
enum {
    COL_LAT, COL_LNG, COL_DESC, COL_ZIP, COL_TITLE,
    COL_TIMESTAMP, COL_TWP, COL_ADDR, COL_E, NUM_COLUNAS
};

// Separa a linha nas vírgulas, trocando cada vírgula por '\0'. 
// Retorna quantos campos achou.

static int separar_campos(char *linha, char *campos[]) {
    int n = 0;
    char *p = linha;

    while (n < NUM_COLUNAS) {
        campos[n++] = p; //guarda onde o campo atual começa
        p = strchr(p, ','); //procua a prox ,
        if (p == NULL) {
            break;
        }
        *p = '\0'; //troca a, por \0 pra saber onde termina o campo
        p++;
    }
    return n;
}

// Converte "2015-12-10 17:10:52" em 20151210171052. Retorna 0 se o formato for inválido.
static int converter_data(const char *texto, long long *data_hora) {
    int ano, mes, dia, hora, minuto, segundo;

    if (sscanf(texto, "%d-%d-%d %d:%d:%d", &ano, &mes, &dia, &hora, &minuto, &segundo) != 6) {
        return 0; //sscanf devolve quantos números conseguiu ler
    }
    *data_hora = ano * 10000000000LL + mes * 100000000LL + dia * 1000000LL
               + hora * 10000LL + minuto * 100LL + segundo;
    return 1;
}

int csv_carregar(const char *caminho, Ocorrencia *vetor, int max) {
    FILE *arquivo = fopen(caminho, "r");
    if (arquivo == NULL) {
        return -1;
    }

    char linha[TAM_LINHA];
    char *campos[NUM_COLUNAS]; // 9 ponteiros um pra cada campo
    int lidas = 0;
    int num_linha = 1; //diz a posicao na linha que deu problema

    fgets(linha, sizeof linha, arquivo); // pula o cabeçalho

    while (lidas < max && fgets(linha, sizeof linha, arquivo) != NULL) {
        num_linha++;
        linha[strcspn(linha, "\r\n")] = '\0'; // tira a quebra de linha (\n ou \r\n)

        Ocorrencia *o = &vetor[lidas]; //faz o o apontar para a próxima posição livre do vetor
        memset(o, 0, sizeof *o); // zera tudo; 

        if (separar_campos(linha, campos) != NUM_COLUNAS || !converter_data(campos[COL_TIMESTAMP], &o->data_hora)) {
            fprintf(stderr, "Aviso: linha %d do CSV ignorada (formato inválido)\n", num_linha);
            continue;
        }

        o->id = lidas + 1; //numera as ocorrências 1, 2, 3
        o->lat = atof(campos[COL_LAT]);
        o->lng = atof(campos[COL_LNG]);
        snprintf(o->tipo, sizeof o->tipo, "%s", campos[COL_TITLE]); //snprint e copiar o texto de um campo do CSV para dentro da struct
        snprintf(o->descricao, sizeof o->descricao, "%s", campos[COL_DESC]);
        snprintf(o->regiao, sizeof o->regiao, "%s", campos[COL_TWP]);
        snprintf(o->endereco, sizeof o->endereco, "%s", campos[COL_ADDR]);
        snprintf(o->cep, sizeof o->cep, "%s", campos[COL_ZIP]);
        lidas++;
    }

    fclose(arquivo);
    return lidas;
}
