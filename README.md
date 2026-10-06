# Central de Emergências: Operação Resgate

Trabalho 1 de Algoritmos e Estruturas de Dados II (UFPel).

Sistema em C que organiza as ocorrências de uma central de emergências, faz consultas rápidas, verifica a integridade dos registros e ajuda a definir a ordem de atendimento. Usa três técnicas implementadas do zero: **Tabela Hash**, **Árvore B+** e **Algoritmo Guloso**.

## Integrantes

- João Pedro Guerreiro Lima
- Mariana Ferreira Tica

## Repositório

https://github.com/marianatica/gerenciador-central-de-emergencias

## Como executar

Pré-requisito: compilador **GCC**. Os comandos devem ser executados **na pasta raiz do projeto**, porque o programa lê o arquivo `data/ocorrencias_911.csv`.

**Windows** (compila e já executa):

```bat
build.bat
```

No PowerShell: `.\build.bat`.

**Linux / macOS** (ou Windows sem o `build.bat`):

```bash
gcc -Iinclude src/main.c -o central
./central
```

O programa é compilado a partir da `main.c`, que inclui o código de todos os módulos (`#include "hash.c"`, `#include "btree.c"` etc.). Por isso basta compilar esse arquivo. O `-Iinclude` indica onde estão os arquivos `.h`.

Ao abrir, o sistema carrega as ocorrências do CSV e mostra o menu principal, com um item para cada módulo.

## Fonte de dados

| Item | Descrição |
|---|---|
| Fonte | **911 Calls – Emergency Dataset** (Montgomery County, Pensilvânia, EUA), publicado no Kaggle |
| Link | https://www.kaggle.com/datasets/mchirico/montcoalert |
| Arquivo usado | `data/ocorrencias_911.csv` |
| Registros | **150**: as 150 primeiras chamadas do arquivo `911.csv` (de 10/12/2015 às 17h até 11/12/2015 às 6h) |

**Campos utilizados** (todos vêm direto do dataset):

| Coluna do CSV | Campo no sistema | Conteúdo |
|---|---|---|
| `title` | `tipo` | categoria e tipo da ocorrência (ex.: `EMS: CARDIAC EMERGENCY`) |
| `desc` | `descricao` | descrição da chamada |
| `twp` | `regiao` | município |
| `addr` | `endereco` | endereço |
| `zip` | `cep` | código postal (vazio em 16 registros) |
| `lat`, `lng` | `lat`, `lng` | coordenadas |
| `timeStamp` | `data_hora` | data e hora de abertura (`AAAA-MM-DD hh:mm:ss`) |

**Campos acrescentados pelo sistema:**

- `id`: o dataset não tem identificador. Cada ocorrência recebe o número da sua linha no CSV (1 a 150), e as cadastradas pelo menu recebem o próximo número livre.
- `assinatura` e `versao`: o identificador baseado no conteúdo, exigido pelo Módulo 4. Ficam guardados na Tabela Hash, junto de cada ocorrência, e não nos dados da ocorrência.

**Adaptações:**

- Foi usada uma amostra de 150 registros (acima dos 100 recomendados), porque o arquivo completo, com cerca de 660 mil chamadas, é grande demais para o repositório.
- A coluna `e` foi ignorada, porque vale sempre 1.
- Os campos sugeridos no enunciado que não existem no dataset (prioridade, status, pessoas envolvidas, tempo estimado e equipe) **não foram criados**. O enunciado diz que esses dados "poderão" ser usados, e optamos por trabalhar só com o que o dataset tem.
- A data é mantida como texto `AAAA-MM-DD hh:mm:ss`. Como o formato tem largura fixa e zeros à esquerda, a ordem alfabética é a ordem cronológica, e a data pode ser comparada com `strcmp` e usada direto como chave da Árvore B+.
- Linhas do CSV com formato inválido (número de colunas errado ou data fora do formato) são ignoradas na leitura, e o sistema avisa quantas foram.
- As buscas por texto diferenciam maiúsculas de minúsculas: os valores devem ser digitados como estão no dataset (ex.: `NORRISTOWN`, `Fire`).

## Organização do código

| Arquivo | Responsabilidade |
|---|---|
| `src/main.c` | carrega o CSV na Tabela Hash e nas Árvores B+ e mostra o menu principal |
| `src/csv.c` | lê o arquivo CSV |
| `src/hash.c` | **Tabela Hash** (armazenamento principal e assinatura de integridade) |
| `src/btree.c` | **Árvore B+** |
| `src/indices.c` | as três Árvores B+ usadas como índices (tipo, região e data) |
| `src/greedy.c` | **Algoritmo Guloso** (Módulo 5) |
| `src/central.c` | Módulo 1 – Central de Ocorrências |
| `src/consultas.c` | Módulo 2 – Consulta Rápida e Módulo 3 – Organização das Ocorrências |
| `src/integridade.c` | Módulo 4 – Integridade dos Registros |
| `src/entrada.c` | leitura do teclado com validação, usada por todos os módulos |
| `include/*.h` | as declarações de cada arquivo `.c` e a `struct Ocorrencia` (`ocorrencia.h`) |

## Estruturas de dados e algoritmos

Das quatro técnicas do enunciado, foram implementadas **Tabela Hash**, **Árvore B+** e **Algoritmo Guloso**, todas do zero, sem bibliotecas prontas.

### Tabela Hash (`src/hash.c`)

**Onde é usada:** é o armazenamento principal das ocorrências (Módulo 1), a busca por id (Módulo 2) e a base da verificação de integridade (Módulo 4).

**Como funciona:** a posição de cada ocorrência é `id % 223`. Ocorrências que caem na mesma posição ficam numa lista ligada (encadeamento), e a nova entra no começo da lista.

| Decisão | Justificativa |
|---|---|
| Tabela Hash para guardar as ocorrências | consultar, alterar e remover começam por achar a ocorrência pelo id. A hash vai direto à posição certa, em tempo constante no caso médio, em vez de fazer uma busca sequencial |
| **223 posições** | é o menor número primo que deixa o fator de carga abaixo de 0,7 com as 150 ocorrências (150 / 223 = 0,67). Com fator de carga baixo, as listas ficam curtas; e um tamanho primo espalha melhor os restos da divisão |
| **Encadeamento** (e não endereçamento aberto) | o Módulo 1 exige remover: no encadeamento, a remoção é física, sem precisar de marcas de "removido" (lápides). A tabela também nunca "enche" com novos cadastros, e uma colisão não ocupa as posições vizinhas |
| **Assinatura do conteúdo** com a função **djb2** (`h = 5381`; para cada letra, `h = h * 33 + letra`) | é o identificador baseado no conteúdo exigido pelo Módulo 4: qualquer letra diferente muda o número. O id fica fora da conta, para que duas ocorrências com o mesmo conteúdo tenham a mesma assinatura (duplicatas) |

### Árvore B+ (`src/btree.c`)

**Onde é usada:** três árvores servem de índice por **tipo**, **região** e **data** (Módulos 2 e 3, e a restrição por região do Módulo 5). Uma árvore temporária é usada no Módulo 4 para encontrar duplicatas e conflitos.

**Como funciona:** cada nó guarda até `m = 4` chaves em ordem. Os nós internos só orientam o caminho, e os dados ficam nas folhas, que são ligadas em sequência. Cada entrada é um par (texto, id), e a ocorrência completa continua na Tabela Hash. Quando um nó passa de 4 chaves, ele se divide em dois: numa folha, sobe para o pai uma cópia da primeira chave da nova folha; num nó interno, sobe a chave do meio. A árvore só cresce pela raiz, então todas as folhas ficam sempre no mesmo nível.

| Decisão | Justificativa |
|---|---|
| Árvore B+ para tipo, região e data | o Módulo 2 pede busca por prefixo, e as consultas por intervalo de datas precisam de ordem. A Tabela Hash espalha os dados e não consegue fazer isso. A B+ mantém tudo em ordem |
| **B+** (e não a árvore B comum) | com as folhas encadeadas, uma busca por prefixo ou por intervalo desce uma vez até a primeira chave e depois só anda pelas folhas. Na árvore B, os dados também ficam nos nós internos, e seria preciso subir e descer pela árvore |
| Chave **(texto, id)** | várias ocorrências têm o mesmo tipo ou a mesma região. O id desempata, então cada entrada é única, e a remoção apaga exatamente a ocorrência certa |
| **Ordem m = 4** (no máximo 4 chaves e 5 filhos por nó) | com 150 ocorrências, a árvore fica com 4 níveis, e as divisões de nós acontecem de verdade. Com ordens grandes, a árvore teria só a raiz e as folhas. O valor é uma constante (`ORDEM`) e pode ser alterado |
| **Remoção sem rebalanceamento** | a remoção tira a chave da folha, sem juntar nós com os vizinhos. As buscas continuam corretas, porque as chaves dos nós internos continuam indicando o caminho. O custo é que algumas folhas podem ficar com poucas chaves. O rebalanceamento completo é a parte mais complexa da B+ e não muda o resultado das buscas |

### Algoritmo Guloso (`src/greedy.c`)

**Onde é usado:** no Módulo 5 (Operação Resgate), para definir a ordem de atendimento.

**Como funciona:**

1. Os **candidatos** são as ocorrências que atendem às restrições informadas: região (encontrada pela Árvore B+) e/ou categoria (EMS, Fire ou Traffic).
2. A equipe sai da **base em Norristown**, a sede do condado.
3. **A cada passo, o algoritmo escolhe a melhor ocorrência entre as que faltam**, segundo o critério escolhido, vai até ela e não volta atrás.
4. Ele para quando todas foram atendidas ou quando a próxima escolhida ultrapassaria o **limite de quilômetros** que a equipe pode percorrer.
5. Cada linha da ordem mostra **o motivo da escolha** e a distância percorrida.

**Critérios de prioridade:**

| Critério | Escolhe primeiro | Em caso de empate |
|---|---|---|
| 1 – Mais antiga | a ocorrência aberta há mais tempo (maior espera) | — |
| 2 – Mais próxima | a mais perto de onde a equipe está agora (vizinho mais próximo) | a mais antiga |
| 3 – Combinado | a categoria mais urgente: **EMS** (risco à vida), depois **Fire**, depois **Traffic** | a mais antiga |

| Decisão | Justificativa |
|---|---|
| Algoritmo guloso | numa central, a decisão precisa ser rápida e explicável: cada escolha é a melhor naquele momento, pelo critério declarado, e o sistema mostra o motivo |
| Parar no limite de km, em vez de pular para uma ocorrência mais perto | respeita a ordem do critério: uma ocorrência menos prioritária não passa à frente de uma mais prioritária |
| Distância aproximada: 1 grau de latitude = 111 km e 1 grau de longitude = 85 km, somando os deslocamentos norte-sul e leste-oeste | na latitude do condado (cerca de 40 graus), 1 grau de longitude mede 111 × cos(40°) ≈ 85 km. A soma dos dois eixos se aproxima de um deslocamento por ruas, e não usa nenhuma biblioteca |
| Limitação conhecida | o guloso não garante a melhor solução global: o vizinho mais próximo, por exemplo, não garante o menor percurso total. Em troca, é simples e rápido (O(n²) para n candidatos) |

## Módulos

### Módulo 1 – Central de Ocorrências

| Funcionalidade | Como é feita |
|---|---|
| carregar os registros iniciais | ao abrir, o CSV é lido e cada ocorrência entra na Tabela Hash e nas três Árvores B+ |
| cadastrar | a nova ocorrência recebe o próximo id livre e entra na hash e nas árvores |
| consultar | busca pelo id na Tabela Hash |
| alterar | o campo é alterado na hash; as árvores trocam o valor antigo pelo novo; a assinatura é recalculada e a versão aumenta (alteração autorizada) |
| remover | sai das árvores e da hash (com confirmação) |
| listar | mostra todas as ocorrências da Tabela Hash |

### Módulo 2 – Consulta Rápida (Modo Consulta Rápida)

| Critério | Estrutura usada |
|---|---|
| identificador | Tabela Hash |
| palavra da descrição | busca sequencial (ver justificativa abaixo) |
| tipo | Árvore B+ do tipo |
| região | Árvore B+ da região |
| prefixo (do tipo, ex.: `Traffic` ou `EMS`) | Árvore B+ do tipo |
| intervalo de datas (critério definido pelo grupo) | Árvore B+ da data |

Cada busca mostra quantas ocorrências ou nós a estrutura precisou olhar, comparado com a busca sequencial. Exemplos com a amostra de 150 ocorrências:

| Busca | Resultado | Estrutura | Busca sequencial |
|---|---|---|---|
| id 42 | 1 ocorrência | 1 comparação (Tabela Hash) | até 150 |
| tipo `Fire: FIRE ALARM` | 10 ocorrências | 9 nós (Árvore B+) | 150 |
| região `NORRISTOWN` | 14 ocorrências | 11 nós (Árvore B+) | 150 |
| prefixo `Traffic` | 47 ocorrências | 25 nós (Árvore B+) | 150 |
| datas de 18:00 a 18:30 de 10/12 | 14 ocorrências | 13 nós (Árvore B+) | 150 |

A busca por **palavra da descrição** é sequencial porque a palavra pode estar no meio do texto. As Árvores B+ são ordenadas pelo começo do texto e só aceleram buscas por prefixo; nenhuma das estruturas escolhidas acelera a busca de uma palavra no meio de um texto.

### Módulo 3 – Organização das Ocorrências

| Consulta | Como é feita |
|---|---|
| ocorrências de uma região | Árvore B+ da região |
| ocorrências de uma categoria (EMS, Fire ou Traffic) | prefixo na Árvore B+ do tipo |
| todas em ordem cronológica | percorre todas as folhas da Árvore B+ da data, que já estão em ordem |
| todas em ordem de região | percorre todas as folhas da Árvore B+ da região |

Sobre os exemplos de consulta do enunciado: "quais ocorrências estão na região X" e "quais são de uma categoria" estão neste módulo. "Quais são acidentes" é respondida pela busca por prefixo do tipo (Módulo 2, por exemplo `Traffic: VEHICLE ACCIDENT`). "Pendentes", "prioridade elevada" e "mais de uma pessoa" não se aplicam, porque o dataset não tem status, prioridade nem quantidade de pessoas (ver [Fonte de dados](#fonte-de-dados)).

### Módulo 4 – Integridade dos Registros (Modo Investigação)

| Funcionalidade | Como é feita |
|---|---|
| verificar se uma ocorrência foi alterada desde a inserção | recalcula a assinatura e compara com a guardada no cadastro (ou na última alteração autorizada) |
| verificar todas as ocorrências | a mesma verificação para a base inteira |
| registros inconsistentes | procura campos vazios (tipo, região, endereço ou CEP) e coordenadas fora da área do condado (latitude entre 39,9 e 40,6; longitude entre -75,8 e -74,9). Na amostra, aparecem os 16 registros sem CEP |
| duplicadas e conflitos de versão | ocorrências com a mesma data e o mesmo endereço são o mesmo evento. Elas entram numa Árvore B+ temporária com essa chave e ficam lado a lado nas folhas. Mesma assinatura indica **duplicada**; assinatura diferente indica **conflito de versões**. Assim, cada ocorrência é comparada só com a seguinte, em vez de comparar todos os pares |
| comparar com o arquivo carregado | relê o CSV e compara a assinatura de cada ocorrência do arquivo com a que está na memória, apontando as diferentes, as removidas e as cadastradas depois |
| simular alteração não autorizada | muda a região de uma ocorrência sem registrar a nova assinatura, para demonstrar que o sistema detecta a alteração |

A amostra não tem duplicatas. Para demonstrar a detecção, basta cadastrar pelo Módulo 1 uma ocorrência com a mesma data e o mesmo endereço de uma já existente.

### Módulo 5 – Operação Resgate (Modo Operação Resgate)

| Exigência | Como é atendida |
|---|---|
| selecionar ocorrências de acordo com restrições | restrição por região e/ou categoria |
| estabelecer uma ordem de atendimento | Algoritmo Guloso |
| considerar diferentes critérios de prioridade | mais antiga e mais próxima |
| combinar mais de um critério | critério combinado: categoria e antiguidade |
| limite de tempo ou recurso | limite de quilômetros que a equipe pode percorrer |
| decisões justificáveis | cada escolha mostra o motivo e a distância |

Os critérios sugeridos pelo enunciado de nível de prioridade, quantidade de pessoas e tempo estimado não existem no dataset. Por isso, o grupo usou os critérios que os dados permitem: **região**, **tipo** (categoria), **data/hora de abertura** (tempo de espera) e **localização** (distância até a equipe).

## Bibliotecas utilizadas

Apenas a biblioteca padrão do C, como apoio:

- `stdio.h`
- `stdlib.h`
- `string.h`

