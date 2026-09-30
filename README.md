# Central de Emergências: Operação Resgate

Sistema para organizar ocorrências de uma central de emergências, realizar consultas rápidas, auxiliar na definição da ordem de atendimento e verificar a integridade das informações armazenadas.

## Integrantes

- João Pedro Guerreiro Lima
- Mariana Ferreira Tica

## Repositório

https://github.com/marianatica/gerenciador-central-de-emergencias

## Status

**Em planejamento.** O planejamento completo, com a justificativa de cada decisão, ficara em
[docs/PLANO.md](docs/PLANO.md).

## Fonte de dados

- **Dataset:** 911 Calls – Emergency Dataset 
- **Link:** https://www.kaggle.com/datasets/mchirico/montcoalert
- **Registros utilizados:** amostra de 1.000 chamadas, pois o arquivo completo e muito grande

## Estruturas de dados e algoritmos

| Técnica | Onde será aplicada | Justificativa |
|---|---|---|
| **Tabela Hash** | Armazenamento principal (ID → ocorrência); índices por tipo, região, status e equipe; índice invertido de palavras da descrição; detecção de duplicatas e lacre de integridade | Busca exata em O(1) no caso médio, sem percorrer a base inteira |
| **Árvore B+** | Índices ordenados por prioridade, data/hora, pessoas envolvidas, endereço e tipo; listagem em ordem cronológica | As folhas encadeadas permitem consultas por faixa e por **prefixo** em O(log n + k) |
| **Algoritmo Guloso** | Ordem de atendimento, seleção de ocorrências dentro de um limite de tempo e distribuição entre equipes | Decisões rápidas e explicáveis passo a passo, com base em critérios de prioridade declarados |

## Linguagem

C 

## Como executar

Será descrito quando a implementação estiver pronta.
