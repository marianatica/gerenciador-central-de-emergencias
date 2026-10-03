#ifndef OCORRENCIA_H
#define OCORRENCIA_H

#define TAM_TIPO      50
#define TAM_DESCRICAO 150
#define TAM_REGIAO    50
#define TAM_ENDERECO  100
#define TAM_CEP       8
#define TAM_DATA      20   // "2015-12-10 17:10:52": 19 caracteres + '\0'

typedef struct {
    int id;                        // número da linha no CSV (o dataset não tem identificador)
    char tipo[TAM_TIPO];           // title
    char descricao[TAM_DESCRICAO]; // desc
    char regiao[TAM_REGIAO];       // twp
    char endereco[TAM_ENDERECO];   // addr
    char cep[TAM_CEP];             // zip
    double lat;                    // lat
    double lng;                    // lng
    char data_hora[TAM_DATA];      // timeStamp
} Ocorrencia;

#endif
