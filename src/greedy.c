#include <stdio.h>
#include <string.h>

#include "greedy.h"
#include "entrada.h"

#define MAX_OCORRENCIAS 1000

// Base da equipe: Norristown, sede do condado de Montgomery (de onde vêm as chamadas).
#define BASE_LAT 40.1215
#define BASE_LNG -75.3399

// Quantos km vale 1 grau na região do condado (latitude de cerca de 40 graus):
// 1 grau de latitude = 111 km; 1 grau de longitude = 111 x cos(40 graus) = 85 km.
#define KM_POR_GRAU_LAT 111.0
#define KM_POR_GRAU_LNG 85.0

static double valor_absoluto(double x) {
    if (x < 0) {
        return -x;
    }
    return x;
}

// Distância aproximada em km: soma o deslocamento norte-sul com o leste-oeste,
// como quem anda pelas ruas (sem cortar caminho em diagonal).
static double distancia_km(double lat1, double lng1, double lat2, double lng2) {
    return valor_absoluto(lat1 - lat2) * KM_POR_GRAU_LAT + valor_absoluto(lng1 - lng2) * KM_POR_GRAU_LNG;
}

// Prioridade da categoria: 1 = EMS (risco à vida), 2 = Fire, 3 = Traffic, 4 = outras.
static int prioridade_categoria(Ocorrencia *o) {
    if (strncmp(o->tipo, "EMS", 3) == 0) {
        return 1;
    }
    if (strncmp(o->tipo, "Fire", 4) == 0) {
        return 2;
    }
    if (strncmp(o->tipo, "Traffic", 7) == 0) {
        return 3;
    }
    return 4;
}

// Diz se a ocorrência a deve ser atendida antes da b, pelo critério escolhido.
// (lat, lng) é onde a equipe está agora. Em empate, vem primeiro a mais antiga.
static int vem_antes(Ocorrencia *a, Ocorrencia *b, int criterio, double lat, double lng) {
    if (criterio == 2) {
        double da = distancia_km(lat, lng, a->lat, a->lng);
        double db = distancia_km(lat, lng, b->lat, b->lng);
        if (da < db) {
            return 1;
        }
        if (da > db) {
            return 0;
        }
    } else if (criterio == 3) {
        int pa = prioridade_categoria(a);
        int pb = prioridade_categoria(b);
        if (pa < pb) {
            return 1;
        }
        if (pa > pb) {
            return 0;
        }
    }
    // critério 1, ou empate nos critérios 2 e 3: a mais antiga primeiro
    if (strcmp(a->data_hora, b->data_hora) < 0) {
        return 1;
    }
    return 0;
}

// Restrições: separa as ocorrências da região (pela Árvore B+) e da categoria pedidas.
// Região ou categoria vazia = sem essa restrição. Retorna quantos candidatos ficaram.
static int selecionar(TabelaHash *tabela, Indices *indices, char *regiao, char *categoria, int *candidatos) {
    int ids[MAX_OCORRENCIAS];
    int n;

    if (regiao[0] != '\0') {
        n = btree_buscar_faixa(&indices->regiao, regiao, regiao, ids, MAX_OCORRENCIAS);
    } else {
        n = hash_todos_ids(tabela, ids, MAX_OCORRENCIAS);
    }

    int total = 0;
    for (int i = 0; i < n; i++) {
        Ocorrencia *o = hash_buscar(tabela, ids[i]);
        if (o != NULL && strncmp(o->tipo, categoria, strlen(categoria)) == 0) {
            candidatos[total] = ids[i];
            total++;
        }
    }
    return total;
}

// Algoritmo guloso: a cada passo escolhe a melhor ocorrência entre as que faltam (pelo critério),
// vai até ela e não volta atrás. Para quando acabam as ocorrências ou o limite de km.
static void operacao_resgate(TabelaHash *tabela, Indices *indices, int criterio) {
    char regiao[TAM_REGIAO];
    char categoria[TAM_TIPO];
    int candidatos[MAX_OCORRENCIAS];
    int atendida[MAX_OCORRENCIAS];

    printf("Restrições (aperte só Enter para não usar):\n");
    ler_texto("Região (ex.: NORRISTOWN): ", regiao, TAM_REGIAO);
    ler_texto("Categoria (EMS, Fire ou Traffic): ", categoria, TAM_TIPO);
    double limite = ler_decimal("Limite de deslocamento da equipe em km (0 = sem limite): ");

    int n = selecionar(tabela, indices, regiao, categoria, candidatos);
    if (n == 0) {
        printf("Nenhuma ocorrência atende às restrições.\n");
        return;
    }
    for (int i = 0; i < n; i++) {
        atendida[i] = 0;
    }

    char *motivo = "a mais antiga entre as que faltam";
    if (criterio == 2) {
        motivo = "a mais próxima de onde a equipe está";
    } else if (criterio == 3) {
        motivo = "a categoria mais urgente e, nela, a mais antiga";
    }

    double lat = BASE_LAT;
    double lng = BASE_LNG;
    double percorrido = 0;
    int atendidas = 0;
    int parar = 0;

    printf("\nOrdem de atendimento (a equipe sai da base em Norristown):\n");
    while (atendidas < n && !parar) {
        // Escolha gulosa: procura a melhor entre as ocorrências que ainda não foram atendidas.
        int melhor = -1;
        for (int i = 0; i < n; i++) {
            if (!atendida[i]) {
                if (melhor == -1 || vem_antes(hash_buscar(tabela, candidatos[i]),
                                              hash_buscar(tabela, candidatos[melhor]), criterio, lat, lng)) {
                    melhor = i;
                }
            }
        }

        Ocorrencia *o = hash_buscar(tabela, candidatos[melhor]);
        double distancia = distancia_km(lat, lng, o->lat, o->lng);

        if (limite > 0 && percorrido + distancia > limite) {
            printf("Parou: a próxima (id %d) fica a %.1f km e passaria do limite de %.1f km.\n",
                   o->id, distancia, limite);
            parar = 1;
        } else {
            atendida[melhor] = 1;
            atendidas++;
            percorrido = percorrido + distancia;
            printf("%2d. id %4d | %s | %-30s | %-18s | %5.1f km | %s\n",
                   atendidas, o->id, o->data_hora, o->tipo, o->regiao, distancia, motivo);
            lat = o->lat; // a equipe agora está nessa ocorrência
            lng = o->lng;
        }
    }

    printf("\n%d de %d ocorrência(s) atendida(s); deslocamento total: %.1f km.\n", atendidas, n, percorrido);
    if (atendidas < n) {
        printf("%d ocorrência(s) ficaram para a próxima equipe.\n", n - atendidas);
    }
}

void greedy_menu(TabelaHash *tabela, Indices *indices) {
    int opcao = -1;

    while (opcao != 0) {
        printf("\n=== MÓDULO 5: OPERAÇÃO RESGATE ===\n");
        printf("Critério para escolher a próxima ocorrência:\n");
        printf("1 - Mais antiga primeiro (maior tempo de espera)\n");
        printf("2 - Mais próxima da equipe\n");
        printf("3 - Combinado: categoria (EMS > Fire > Traffic) e, em empate, a mais antiga\n");
        printf("0 - Voltar\n");
        opcao = ler_inteiro("Opção: ");

        if (opcao >= 1 && opcao <= 3) {
            operacao_resgate(tabela, indices, opcao);
        } else if (opcao != 0) {
            printf("Opção inválida.\n");
        }
    }
}
