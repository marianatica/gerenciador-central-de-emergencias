#include <stdio.h>
#include <stdlib.h>

#include "entrada.h"

#define TAM_NUMERO 50

void ler_texto(char *mensagem, char *texto, int tamanho) {
    printf("%s", mensagem);
    if (fgets(texto, tamanho, stdin) == NULL) {
        texto[0] = '\0';
        return;
    }

    // Procura o Enter do final da linha.
    int i = 0;
    while (texto[i] != '\0' && texto[i] != '\n') {
        i++;
    }

    if (texto[i] == '\n') {
        texto[i] = '\0'; // tira o Enter
    } else {
        // A pessoa digitou mais do que cabe: descarta o resto da linha,
        // senao ele seria lido como a resposta da proxima pergunta.
        int c = getchar();
        while (c != '\n' && c != EOF) {
            c = getchar();
        }
    }
}

// Diz se o texto e um numero inteiro (so digitos, com um '-' opcional no comeco).
static int eh_inteiro(char *texto) {
    int i = 0;
    if (texto[0] == '-') {
        i = 1;
    }
    if (texto[i] == '\0') {
        return 0;
    }
    while (texto[i] != '\0') {
        if (texto[i] < '0' || texto[i] > '9') {
            return 0;
        }
        i++;
    }
    return 1;
}

// Diz se o texto e um numero com casas decimais (digitos, no maximo um '.', '-' opcional).
static int eh_decimal(char *texto) {
    int i = 0;
    int pontos = 0;
    int digitos = 0;
    if (texto[0] == '-') {
        i = 1;
    }
    while (texto[i] != '\0') {
        if (texto[i] == '.') {
            pontos++;
        } else if (texto[i] >= '0' && texto[i] <= '9') {
            digitos++;
        } else {
            return 0;
        }
        i++;
    }
    if (pontos > 1 || digitos == 0) {
        return 0;
    }
    return 1;
}

int ler_inteiro(char *mensagem) {
    char texto[TAM_NUMERO];

    while (1) {
        ler_texto(mensagem, texto, TAM_NUMERO);
        if (eh_inteiro(texto)) {
            return atoi(texto);
        }
        if (feof(stdin)) {
            return 0; // a entrada acabou: devolve 0 (que nos menus significa "voltar")
        }
        printf("Digite um numero inteiro.\n");
    }
}

double ler_decimal(char *mensagem) {
    char texto[TAM_NUMERO];

    while (1) {
        ler_texto(mensagem, texto, TAM_NUMERO);
        if (eh_decimal(texto)) {
            return atof(texto);
        }
        if (feof(stdin)) {
            return 0;
        }
        printf("Digite um numero, usando ponto nas casas decimais (ex.: -75.34).\n");
    }
}
