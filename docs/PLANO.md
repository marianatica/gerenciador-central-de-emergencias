# Central de Emergências: Operação Resgate — Planejamento

## Contexto

Trabalho 1 de AED II, João Pedro Guerreiro Lima e Mariana Ferreira Tica.
Sistema em C com o dataset **911 Calls (Kaggle)**.

As técnicas que utilizaremos são **Tabela Hash**, **Árvore B+** e **Algoritmo Guloso**.

Cada decisão abaixo vem acompanhada da sua justificativa.

## Decisões iniciais

| Decisão | Justificativa |
|---|---|
| Linguagem **C** | É a linguagem da disciplina que temos mais contato, desde AED I. |
| Dataset **911 Calls** (Kaggle) | É o recomendado pelo enunciado e traz tipo, descrição, região, endereço e data/hora das ocorrências. |
| Amostra de **150 registros** | Fica acima do mínimo recomendado (100), e o arquivo completo é muito grande para o GitHub. |
| Interface por **menu no terminal** | O foco da avaliação são as estruturas. Roda em qualquer máquina sem dependências e é simples. |

## Estruturas escolhidas

| Técnica | Onde será aplicada | Justificativa |
|---|---|---|
| **Tabela Hash** | Busca por ID; índices por tipo, região e status | Busca exata em O(1) no caso médio. |
| **Árvore B+** | Consultas por faixa (prioridade, data) e por prefixo (endereço, tipo) | As folhas encadeadas permitem percorrer intervalos em ordem; é isso que resolve a busca por prefixo, já que não usamos Trie. |
| **Algoritmo Guloso** | Ordem de atendimento e seleção de ocorrências dentro de um limite de tempo | Decide rápido, e cada escolha pode ser explicada pelo critério usado. |

## Módulos

- [ ] **Módulo 1 – Central de Ocorrências:** carregar, cadastrar, consultar, alterar, remover e listar
- [ ] **Módulo 2 – Consulta Rápida:** por ID, descrição, tipo, região e prefixo
- [ ] **Módulo 3 – Organização:** filtros por região, tipo, status, prioridade e pessoas envolvidas
- [ ] **Módulo 4 – Integridade:** identificador baseado no conteúdo, duplicatas e conflitos
- [ ] **Módulo 5 – Operação Resgate:** ordem de atendimento com critérios combinados

Modos de interação: Investigação, Operação Resgate e Consulta Rápida (reaproveitam os módulos acima).

## Próximos passos

1. Baixar o dataset e gerar a amostra de 150 registros
2. Definir a estrutura de uma ocorrência e ler o CSV
3. Implementar a Tabela Hash
4. Implementar a Árvore B+
5. Implementar o Algoritmo Guloso
6. Implementar a verificação de integridade
