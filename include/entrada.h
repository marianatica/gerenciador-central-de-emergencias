#ifndef ENTRADA_H
#define ENTRADA_H

// Lê uma linha digitada (sem o Enter), guardando no máximo tamanho - 1 letras.
void ler_texto(char *mensagem, char *texto, int tamanho);

// Lê um número inteiro; repete a pergunta até a pessoa digitar um número válido.
int ler_inteiro(char *mensagem);

// Lê um número com casas decimais (usado nas coordenadas); repete até ser válido.
double ler_decimal(char *mensagem);

#endif
