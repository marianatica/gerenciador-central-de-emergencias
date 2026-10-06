#ifndef ENTRADA_H
#define ENTRADA_H

// Le uma linha digitada (sem o Enter), guardando no maximo tamanho - 1 letras.
void ler_texto(char *mensagem, char *texto, int tamanho);

// Le um numero inteiro; repete a pergunta ate a pessoa digitar um numero valido.
int ler_inteiro(char *mensagem);

// Le um numero com casas decimais (usado nas coordenadas); repete ate ser valido.
double ler_decimal(char *mensagem);

#endif
