# Sistema de Distribuicao de Agua

Projecto em C para representar uma rede de distribuicao de agua usando grafos.
Cada estacao e um vertice e cada cano e uma ligacao direccionada entre duas
estacoes, com uma capacidade associada.

## Funcionalidades

- Adicionar estacoes a rede.
- Adicionar canos entre estacoes.
- Listar as estacoes cadastradas.
- Guardar a rede no ficheiro `dados_rede.txt`.
- Carregar a rede a partir do ficheiro `dados_rede.txt`.
- Encontrar uma rota entre duas estacoes usando busca em largura (BFS).

## Estrutura do projecto

```text
.
|-- main.c          # Menu principal e interaccao com o utilizador
|-- grafo.c         # Implementacao das operacoes da rede
|-- grafo.h         # Tipos abstratos
|-- dados_rede.txt  # Dados persistidos da rede
|-- README.md       # Documentacao do projecto
```

## Menu do programa

Ao executar o programa, sera apresentado o seguinte menu:

```text
===== MENU DA REDE DE DISTRIBUICAO DE AGUA =====
1. Adicionar estacao
2. Adicionar cano
3. Listar estacoes
4. Guardar dados no ficheiro
5. Carregar rede a partir do ficheiro
6. Encontrar rota de agua
0. Sair
```

## Formato do ficheiro de dados

O programa guarda e carrega os dados no ficheiro `dados_rede.txt`.

Formato:

```text
quantidade_de_estacoes
nome_da_estacao;tipo_da_estacao
...
quantidade_de_canos
id_origem;id_destino;capacidade
...
```

Exemplo:

```text
5
ETA Luanda Sudeste;ETA
CD Sapu;CD
CD Morro Bento;CD
Bairro Futungo;Bairro
Bairro Gamek;Bairro
4
0;1;90000
1;4;12000
1;2;50000
2;3;15000
```

## Observacoes

- O limite maximo de estacoes esta definido em `grafo.h` pela constante
  `MAX_ESTACOES`, actualmente com valor `50`.
- Os IDs das estacoes sao atribuidos automaticamente pela ordem de cadastro,
  comecando em `0`.
- Os canos sao direccionados: um cano de `0` para `1` nao cria
  automaticamente uma ligacao de `1` para `0`.
- A capacidade do cano deve ser um numero inteiro positivo.

## Exemplo de uso

1. Execute o programa.
2. Escolha a opcao `5` para carregar a rede existente de `dados_rede.txt`.
3. Escolha a opcao `3` para listar as estacoes.
4. Escolha a opcao `6` para procurar uma rota entre duas estacoes.
5. Escolha a opcao `0` para sair.
