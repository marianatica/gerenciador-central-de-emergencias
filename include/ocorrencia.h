#ifndef OCORRENCIA_H
#define OCORRENCIA_H

#define TAM_TIPO      50   
#define TAM_DESCRICAO 150  
#define TAM_REGIAO    50   
#define TAM_ENDERECO  100   
#define TAM_CEP       8    
#define TAM_DATA      20   // "2015-12-10 17:10:52": 19 caracteres + '\0'
#define TAM_EQUIPE    50   


typedef enum {
    STATUS_PENDENTE, //0
    STATUS_EM_ATENDIMENTO, //1
    STATUS_CONCLUIDA //2
} StatusOcorrencia;

typedef struct {
    int id;
    char tipo[TAM_TIPO];           
    char descricao[TAM_DESCRICAO]; 
    char regiao[TAM_REGIAO];       
    char endereco[TAM_ENDERECO];  
    char cep[TAM_CEP];           
    double lat;                   
    double lng;                 
    char data_hora[TAM_DATA];                                    
    int prioridade;                
    int tempo_estimado_min;       
    int pessoas;                   // quantidade de pessoas envolvidas
    StatusOcorrencia status;
    char equipe[TAM_EQUIPE];      
} Ocorrencia;

#endif
