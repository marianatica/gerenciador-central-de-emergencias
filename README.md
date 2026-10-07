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
| Registros | **150**: as 150 primeiras chamadas do arquivo `911.csv` |

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

Das quatro técnicas do enunciado, escolhemos a **Tabela Hash**, a **Árvore B+** e o **Algoritmo Guloso**, e implementamos as três do zero, sem bibliotecas prontas. Cada uma resolve um problema diferente da central.

### Tabela Hash (`src/hash.c`)

Usamos a Tabela Hash para guardar as ocorrências (Módulo 1), para a busca por id (Módulo 2) e como base da verificação de integridade (Módulo 4).

Escolhemos a hash porque quase tudo no Módulo 1 começa por achar uma ocorrência pelo id: consultar, alterar e remover. Numa lista ou num vetor, seria preciso olhar as ocorrências uma a uma até achar o id (busca sequencial), que é justamente o problema que o enunciado descreve. Na hash, uma conta (`id % 223`) diz em que posição a ocorrência está, e a busca vai direto até ela, em tempo constante no caso médio.

Escolhemos 223 posições porque é o menor número primo que deixa o fator de carga abaixo de 0,7 com as 150 ocorrências (150 / 223 = 0,67). Com a tabela menos cheia, quase toda posição guarda no máximo uma ocorrência, e a busca continua rápida. O tamanho é primo porque um primo não tem divisores em comum com padrões nos ids, então os restos da divisão se espalham melhor pela tabela.

Quando dois ids caem na mesma posição (colisão), usamos encadeamento: cada posição tem uma lista ligada, e a ocorrência nova entra no começo dela. Escolhemos o encadeamento, e não o endereçamento aberto, porque o Módulo 1 exige remover ocorrências. No encadeamento, a remoção é simples: o nó sai da lista e a memória é liberada. No endereçamento aberto, seria preciso deixar uma marca de "removido" (lápide) para não quebrar as buscas. Além disso, com encadeamento a tabela nunca enche com novos cadastros, e uma colisão não ocupa a posição vizinha.

Para o Módulo 4, a hash também guarda a assinatura de cada ocorrência, que é o identificador baseado no conteúdo pedido pelo enunciado. Ela é calculada com a função djb2, que começa em 5381 e, para cada letra de todos os campos, faz `h = h * 33 + letra`. Escolhemos a djb2 porque é simples de implementar à mão e qualquer letra diferente muda o resultado. Deixamos o id fora da conta para que duas ocorrências com o mesmo conteúdo tenham a mesma assinatura, e assim dá para achar duplicatas.

### Árvore B+ (`src/btree.c`)

Usamos três Árvores B+ como índices, por tipo, região e data (Módulos 2 e 3 e a restrição por região do Módulo 5), e uma árvore temporária no Módulo 4 para achar duplicatas e conflitos.

Escolhemos a B+ porque o Módulo 2 pede busca por prefixo, e as consultas por intervalo de datas precisam dos dados em ordem. A hash não serve para isso: ela espalha os dados pelas posições, e textos parecidos caem em lugares sem relação nenhuma. A B+ mantém tudo em ordem, então os textos que começam com o mesmo prefixo ficam lado a lado.

Escolhemos a B+, e não a árvore B comum, porque na B+ os dados ficam só nas folhas, e as folhas são ligadas em sequência. Para buscar um prefixo ou um intervalo, a árvore desce uma vez até o primeiro resultado e depois só anda pelas folhas para o lado. Na árvore B, os dados também ficam nos nós internos, e seria preciso subir e descer pela árvore para percorrer um intervalo.

Cada entrada da árvore é um par (texto, id), e a ocorrência completa continua só na hash. Guardamos o id junto porque muitas ocorrências têm o mesmo tipo ou a mesma região (14 são de NORRISTOWN, por exemplo): o id desempata, cada entrada fica única, e a remoção apaga exatamente a ocorrência certa.

Usamos ordem m = 4, ou seja, cada nó guarda no máximo 4 chaves. Quando um nó passa disso, ele se divide em dois: numa folha, sobe para o pai uma cópia da primeira chave da nova folha; num nó interno, sobe a chave do meio. A árvore só cresce pela raiz, então todas as folhas ficam sempre no mesmo nível. Escolhemos uma ordem pequena porque, com 150 ocorrências, a árvore fica com 4 níveis e as divisões acontecem de verdade; com uma ordem grande, ela teria só a raiz e as folhas.

A data fica guardada como texto no formato `AAAA-MM-DD hh:mm:ss`. Como todas as datas têm o mesmo tamanho e zeros à esquerda, a ordem alfabética é a mesma ordem do tempo, e a data serve direto como chave da árvore.

Na remoção, decidimos tirar a chave da folha sem rebalancear a árvore. As buscas continuam corretas, porque as chaves dos nós internos continuam indicando o caminho certo; o custo é que algumas folhas podem ficar com poucas chaves. Fizemos assim porque o rebalanceamento completo é a parte mais complexa da B+ e não muda o resultado das buscas.

### Algoritmo Guloso (`src/greedy.c`)

Usamos o Algoritmo Guloso no Módulo 5, para decidir em que ordem uma equipe deve atender as ocorrências.

Escolhemos um guloso porque testar todas as ordens possíveis é impossível: com 150 ocorrências, são 150! ordens. O guloso decide um passo de cada vez: dentre as ocorrências que faltam, escolhe a melhor naquele momento, vai até ela e não volta atrás. Assim a resposta sai na hora, e o sistema mostra o motivo de cada escolha, que é o que uma central de emergência precisa.

A equipe sai de Norristown, a sede do condado de Montgomery, de onde vêm as chamadas. Antes de começar, o usuário pode restringir as ocorrências a uma região, porque uma equipe normalmente cobre uma área; a busca pela região usa a Árvore B+.

Criamos dois critérios para decidir qual é a melhor ocorrência a cada passo:

- **Mais próxima.** Escolhe a ocorrência mais perto de onde a equipe está agora; depois de atendê-la, a próxima é a mais perto desse novo ponto (vizinho mais próximo). Escolhemos esse critério porque, em emergência, o tempo de chegada importa: indo sempre para a mais próxima, a equipe gasta menos estrada entre um atendimento e outro.
- **Combinado: categoria e, no empate, a mais próxima.** Atende primeiro as ocorrências EMS, depois as Fire e depois as Traffic; entre as da mesma categoria, a mais próxima. Essa ordem é uma decisão nossa: EMS é emergência médica, com risco direto à vida; Fire tem risco à vida e ao patrimônio, mas muitos chamados são alarmes; Traffic, na maioria, não tem vítima grave (quando tem, o dataset registra também um chamado EMS). Criamos este critério porque o enunciado pede para combinar mais de um critério: ele junta a urgência da categoria com a distância.

O usuário também informa um limite de quilômetros, que representa o recurso da equipe (combustível ou tempo de turno). Quando a próxima ocorrência escolhida não cabe no limite, o algoritmo para. Decidimos parar, em vez de pular para outra que coubesse, porque pular quebraria o critério: no critério combinado, por exemplo, uma ocorrência de trânsito passaria na frente de uma emergência médica só por estar mais perto.

Para medir a distância, usamos uma aproximação: 1 grau de latitude vale cerca de 111 km, e 1 grau de longitude, na latitude do condado (cerca de 40 graus), vale 111 × cos(40°) ≈ 85 km. Somamos o deslocamento norte-sul com o leste-oeste, sem cortar em diagonal, porque uma equipe anda por ruas, e não em linha reta. A conta fica só com soma e multiplicação, sem biblioteca.

O guloso tem uma limitação que conhecemos: ele não garante a melhor ordem possível. Achar o menor percurso que passa por todas as ocorrências é o problema do caixeiro-viajante, que não tem solução rápida conhecida. Trocamos a resposta perfeita por uma resposta boa, imediata e explicável, com O(n²) comparações para n ocorrências.

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
| selecionar ocorrências de acordo com restrições | restrição por região |
| estabelecer uma ordem de atendimento | Algoritmo Guloso |
| considerar diferentes critérios de prioridade | mais próxima e categoria |
| combinar mais de um critério | critério combinado: categoria e distância |
| limite de tempo ou recurso | limite de quilômetros que a equipe pode percorrer |
| decisões justificáveis | cada escolha mostra o motivo e a distância |

Os critérios sugeridos pelo enunciado de nível de prioridade, quantidade de pessoas e tempo estimado não existem no dataset. Por isso, o grupo usou os critérios que os dados permitem: **região**, **tipo** (categoria) e **localização** (distância até a equipe).

## Bibliotecas utilizadas

Apenas a biblioteca padrão do C, como apoio:

- `stdio.h`
- `stdlib.h`
- `string.h`

