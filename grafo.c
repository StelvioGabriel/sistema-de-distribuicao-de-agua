#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "grafo.h"

/*
 * Funcao auxiliar para remover as quebras de linha (\n ou \r\n) 
 * que a funcao fgets captura ao ler de ficheiros de texto.
 */
static void remover_quebra_linha(char* texto)
{
    if (texto == NULL) {
        return;
    }
    texto[strcspn(texto, "\r\n")] = '\0';
}

/*
 * Inicializa a estrutura base da Rede de Agua (Grafo).
 * Retorna o ponteiro para a rede ou NULL em caso de falha de memoria.
 */
RedeDeAgua* inicializar_rede()
{
    int i = 0;
    RedeDeAgua* rede = (RedeDeAgua*)malloc(sizeof(RedeDeAgua));
    
    if (rede == NULL) {
        printf("Erro de alocacao de memoria para a rede.\n");
        return NULL;
    }

    rede->num_estacoes = 0;
    // Inicializa todos os ponteiros de estacoes como NULL por seguranca
    for (i = 0; i < MAX_ESTACOES; i++) {
        rede->estacoes[i] = NULL;
    }

    return rede;
}

/*
 * Adiciona um novo vertice (Estacao) ao Grafo.
 * Retorna o ID (indice) da estacao adicionada ou -1 em caso de erro.
 */
int adicionar_estacao(RedeDeAgua* rede, const char* nome, const char* tipo)
{
    // Validacao de seguranca para evitar acessos invalidos
    if (rede == NULL || nome == NULL || tipo == NULL) {
        return -1;
    }

    if (rede->num_estacoes >= MAX_ESTACOES)
    {
        printf("Erro: Limite maximo de estacoes atingido.\n");
        return -1;
    }

    Estacao* nova = (Estacao*)malloc(sizeof(Estacao));
    if (nova == NULL)
    {
        printf("Erro de alocacao de memoria para a estacao.\n");
        return -1;
    }

    strcpy(nova->nome, nome);
    strcpy(nova->tipo, tipo);
    nova->primeiro_cano = NULL; // Inicia a lista de adjacencia vazia

    int id = rede->num_estacoes;
    rede->estacoes[id] = nova;
    rede->num_estacoes++;
    
    return id;
}

/*
 * Adiciona uma nova aresta (Cano) direcionada conectando duas estacoes.
 */
void adicionar_cano(RedeDeAgua* rede, int id_origem, int id_destino, int capacidade)
{
    // Verifica se a rede existe e se os IDs passados estao dentro dos limites validos
    if (rede == NULL || id_origem < 0 || id_destino < 0 ||
        id_origem >= rede->num_estacoes || id_destino >= rede->num_estacoes) {
        printf("Erro: IDs de estacao invalidos para o cano.\n");
        return;
    }

    Cano* novo_cano = (Cano*)malloc(sizeof(Cano));
    if (novo_cano == NULL) {
        printf("Erro de alocacao de memoria.\n");
        return;
    }

    // Insere o novo cano no inicio da lista encadeada (Lista de Adjacencia)
    novo_cano->id_destino = id_destino;
    novo_cano->capacidade = capacidade;
    novo_cano->proximo = rede->estacoes[id_origem]->primeiro_cano;
    rede->estacoes[id_origem]->primeiro_cano = novo_cano;
}

/*
 * Algoritmo de Busca em Largura (BFS) para encontrar se existe
 * um caminho hidrico entre um ponto de origem e um destino.
 */
void encontrar_rota_agua(RedeDeAgua* rede, int id_origem, int id_destino)
{
    if (rede == NULL || id_origem < 0 || id_destino < 0 ||
        id_origem >= rede->num_estacoes || id_destino >= rede->num_estacoes) {
        printf("Erro: Nao foi possivel procurar a rota.\n");
        return;
    }

    int visitados[MAX_ESTACOES] = {0}; // Vetor para evitar ciclos infinitos
    int estacao_anterior[MAX_ESTACOES], i = 0;

    // Inicializa o rastreador de caminhos
    for (i = 0; i < MAX_ESTACOES; i++)
    {
        estacao_anterior[i] = -1;
    }

    int fila[MAX_ESTACOES];
    int inicio = 0, fim = 0;

    fila[fim++] = id_origem;
    visitados[id_origem] = 1;
    int achou_destino = 0;

    // Processamento da Fila BFS
    while (inicio < fim) {
        int atual = fila[inicio++];

        if (atual == id_destino)
        {
            achou_destino = 1;
            break; // Caminho encontrado, interrompe a busca
        }

        // Percorre todos os canos que saem da estacao atual
        Cano* cano_atual = rede->estacoes[atual]->primeiro_cano;
        while (cano_atual != NULL)
        {
            int vizinho = cano_atual->id_destino;
            if (visitados[vizinho] == 0)
            {
                visitados[vizinho] = 1;
                estacao_anterior[vizinho] = atual; // Regista de onde veio a agua
                fila[fim++] = vizinho;
            }
            cano_atual = cano_atual->proximo;
        }
    }

    // Apresentacao dos Resultados (Mensagens de Sucesso ou Falha)
    if (achou_destino)
    {
        printf("\nSUCESSO: Rota encontrada de [%s] para [%s]:\n",
               rede->estacoes[id_origem]->nome, rede->estacoes[id_destino]->nome);

        int caminho[MAX_ESTACOES];
        int tamanho_caminho = 0;
        int rastreador = id_destino, i = 0;

        // Le o vetor de tras para a frente para imprimir a rota correta
        while (rastreador != -1)
        {
            caminho[tamanho_caminho++] = rastreador;
            rastreador = estacao_anterior[rastreador];
        }

        printf("ROTA: ");
        for (i = tamanho_caminho - 1; i >= 0; i--)
        {
            printf("%s", rede->estacoes[caminho[i]]->nome);
            if (i > 0) printf(" -> ");
        }
        printf("\n\n");
    }
    else
    {
        printf("\nFALHA: Nao existe conexao hídrica entre os pontos solicitados.\n");
    }
}

/*
 * Remove uma aresta (Cano) do grafo.
 * Retorna 1 se sucesso, 0 se falhar.
 */
int remover_cano(RedeDeAgua* rede, int id_origem, int id_destino)
{
    if (rede == NULL || id_origem < 0 || id_destino < 0 ||
        id_origem >= rede->num_estacoes || id_destino >= rede->num_estacoes) {
        return 0;
    }

    Cano* atual = rede->estacoes[id_origem]->primeiro_cano;
    Cano* anterior = NULL;

    // Procura o cano na lista encadeada
    while (atual != NULL && atual->id_destino != id_destino) {
        anterior = atual;
        atual = atual->proximo;
    }

    // Se nao encontrou o cano, retorna erro
    if (atual == NULL) {
        return 0; 
    }

    // Desliga o cano da lista encadeada
    if (anterior == NULL) {
        // O cano a remover é o primeiro da lista
        rede->estacoes[id_origem]->primeiro_cano = atual->proximo;
    } else {
        // O cano a remover está no meio ou no fim
        anterior->proximo = atual->proximo;
    }

    free(atual); // Liberta a memoria da aresta
    return 1;
}

/*
 * Remove um vertice (Estacao) do grafo e atualiza todas as ligacoes.
 * Retorna 1 se sucesso, 0 se falhar.
 */
int remover_estacao(RedeDeAgua* rede, int id_estacao)
{
    int i = 0;
    if (rede == NULL || id_estacao < 0 || id_estacao >= rede->num_estacoes) {
        return 0;
    }

    // 1. Remover todos os canos de OUTRAS estacoes que apontam para esta (Arestas de entrada)
    for (i = 0; i < rede->num_estacoes; i++) {
        if (i != id_estacao) {
            remover_cano(rede, i, id_estacao);
        }
    }

    // 2. Libertar a lista de canos DA estacao a ser removida (Arestas de saida)
    Cano* atual = rede->estacoes[id_estacao]->primeiro_cano;
    while (atual != NULL) {
        Cano* temp = atual;
        atual = atual->proximo;
        free(temp);
    }

    // 3. Libertar a estacao em si da memoria
    free(rede->estacoes[id_estacao]);

    // 4. Reorganizar o array para nao deixar buracos (Swap com a ultima estacao)
    int id_ultimo = rede->num_estacoes - 1;
    if (id_estacao != id_ultimo) {
        // Movemos a ultima estacao do array para o espaco que ficou vazio
        rede->estacoes[id_estacao] = rede->estacoes[id_ultimo];
        
        // Agora precisamos de avisar o grafo inteiro que o ID da ultima estacao mudou!
        for (i = 0; i < rede->num_estacoes - 1; i++) { 
            Cano* c = rede->estacoes[i]->primeiro_cano;
            while (c != NULL) {
                if (c->id_destino == id_ultimo) {
                    c->id_destino = id_estacao; // Atualiza o destino para o novo ID
                }
                c = c->proximo;
            }
        }
    }

    // Limpa a ultima posicao e diminui o contador
    rede->estacoes[id_ultimo] = NULL;
    rede->num_estacoes--;

    return 1;
}

/*
 * Imprime todas as ligacoes (canos) existentes no grafo.
 * Mostra a origem, o destino e a capacidade de cada aresta.
 */
void listar_todas_ligacoes(const RedeDeAgua* rede)
{
    int i = 0;
    int encontrou_ligacao = 0;

    if (rede == NULL || rede->num_estacoes == 0) {
        printf("A rede esta vazia. Nenhuma ligacao para mostrar.\n\n");
        return;
    }

    printf("\n--- MAPA DE TODAS AS LIGACOES (CANOS) ---\n");
    for (i = 0; i < rede->num_estacoes; i++) {
        Cano* atual = rede->estacoes[i]->primeiro_cano;
        while (atual != NULL) {
            encontrou_ligacao = 1;
            int destino = atual->id_destino;
            printf("Origem: [%s]  -->  Destino: [%s]  | Capacidade: %d \n", 
                   rede->estacoes[i]->nome, 
                   rede->estacoes[destino]->nome, 
                   atual->capacidade);
            atual = atual->proximo;
        }
    }
    
    if (!encontrou_ligacao) {
        printf("Nenhum cano registado na rede atual.\n");
    }
    printf("-----------------------------------------\n\n");
}

/*
 * Guarda toda a estrutura do grafo (Nos e Arestas) num ficheiro .txt
 * Retorna 1 em caso de sucesso ou 0 em caso de erro.
 */
int guardar_dados_txt(RedeDeAgua* rede, const char* nome_arquivo)
{
    if (rede == NULL || nome_arquivo == NULL) {
        return 0;
    }

    FILE* ficheiro = fopen(nome_arquivo, "w");
    if (ficheiro == NULL) {
        printf("Erro: Nao foi possivel abrir o ficheiro %s para escrita.\n", nome_arquivo);
        return 0;
    }

    int total_canos = 0, i = 0;
    
    // Conta o total de arestas primeiro para formatar corretamente o ficheiro
    for (i = 0; i < rede->num_estacoes; i++) {
        Cano* cano = rede->estacoes[i]->primeiro_cano;
        while (cano != NULL) {
            total_canos++;
            cano = cano->proximo;
        }
    }

    // Escreve os Vertices (Estacoes)
    fprintf(ficheiro, "%d\n", rede->num_estacoes);
    for (i = 0; i < rede->num_estacoes; i++) {
        fprintf(ficheiro, "%s;%s\n", rede->estacoes[i]->nome, rede->estacoes[i]->tipo);
    }

    // Escreve as Arestas (Canos)
    fprintf(ficheiro, "%d\n", total_canos);
    for (i = 0; i < rede->num_estacoes; i++) {
        Cano* cano = rede->estacoes[i]->primeiro_cano;
        while (cano != NULL) {
            fprintf(ficheiro, "%d;%d;%d\n", i, cano->id_destino, cano->capacidade);
            cano = cano->proximo;
        }
    }

    fclose(ficheiro);
    return 1; 
}

/*
 * Carrega a estrutura do grafo a partir de um ficheiro .txt.
 * Aloca a memoria dinamicamente e monta as listas de adjacencia.
 */
RedeDeAgua* carregar_dados_txt(const char* nome_arquivo)
{
    if (nome_arquivo == NULL) {
        return NULL;
    }

    FILE* ficheiro = fopen(nome_arquivo, "r");
    if (ficheiro == NULL) {
        printf("Erro: Nao foi possivel abrir o ficheiro %s para leitura.\n", nome_arquivo);
        return NULL;
    }

    RedeDeAgua* rede = inicializar_rede();
    if (rede == NULL) {
        fclose(ficheiro);
        return NULL;
    }

    char linha[256];
    
    // Leitura do total de estacoes
    if (fgets(linha, sizeof(linha), ficheiro) == NULL) {
        printf("Erro: Ficheiro vazio ou invalido.\n");
        liberar_rede(rede);
        fclose(ficheiro);
        return NULL;
    }

    int total_estacoes = 0, i = 0;
    if (sscanf(linha, "%d", &total_estacoes) != 1 || total_estacoes < 0 || total_estacoes > MAX_ESTACOES) {
        printf("Erro: Quantidade de estacoes invalida no ficheiro.\n");
        liberar_rede(rede);
        fclose(ficheiro);
        return NULL;
    }

    // Leitura dos dados de cada estacao (Vertice)
    for (i = 0; i < total_estacoes; i++) {
        if (fgets(linha, sizeof(linha), ficheiro) == NULL) {
            printf("Erro: Dados de estacao incompletos.\n");
            liberar_rede(rede);
            fclose(ficheiro);
            return NULL;
        }

        remover_quebra_linha(linha);

        char* separador = strchr(linha, ';');
        if (separador == NULL) {
            printf("Erro: Linha de estacao invalida.\n");
            liberar_rede(rede);
            fclose(ficheiro);
            return NULL;
        }

        *separador = '\0'; // Divide a string no local do ponto e virgula
        char* nome = linha;
        char* tipo = separador + 1;

        if (adicionar_estacao(rede, nome, tipo) == -1) {
            liberar_rede(rede);
            fclose(ficheiro);
            return NULL;
        }
    }

    // Leitura do total de canos
    if (fgets(linha, sizeof(linha), ficheiro) == NULL) {
        printf("Erro: Quantidade de canos ausente.\n");
        liberar_rede(rede);
        fclose(ficheiro);
        return NULL;
    }

    int total_canos = 0;
    if (sscanf(linha, "%d", &total_canos) != 1 || total_canos < 0) {
        printf("Erro: Quantidade de canos invalida no ficheiro.\n");
        liberar_rede(rede);
        fclose(ficheiro);
        return NULL;
    }

    // Leitura das ligacoes (Arestas)
    for (i = 0; i < total_canos; i++) {
        if (fgets(linha, sizeof(linha), ficheiro) == NULL) {
            printf("Erro: Dados de cano incompletos.\n");
            liberar_rede(rede);
            fclose(ficheiro);
            return NULL;
        }

        int id_origem = 0, id_destino = 0, capacidade = 0;
        if (sscanf(linha, "%d;%d;%d", &id_origem, &id_destino, &capacidade) != 3) {
            printf("Erro: Linha de cano invalida.\n");
            liberar_rede(rede);
            fclose(ficheiro);
            return NULL;
        }

        adicionar_cano(rede, id_origem, id_destino, capacidade);
    }

    fclose(ficheiro);
    return rede; // Retorna o grafo montado em memoria
}

/*
 * Imprime o conteudo cru do ficheiro .txt no terminal.
 */
void ler_dados_txt(const char* nome_arquivo)
{
    if (nome_arquivo == NULL) {
        return;
    }

    FILE* ficheiro = fopen(nome_arquivo, "r");
    if (ficheiro == NULL) {
        printf("Erro: Nao foi possivel abrir o ficheiro %s para leitura.\n", nome_arquivo);
        return;
    }

    char linha[256];
    printf("\n--- Conteudo do ficheiro %s ---\n", nome_arquivo);
    while (fgets(linha, sizeof(linha), ficheiro) != NULL) {
        printf("%s", linha);
    }
    printf("-----------------------------------\n");

    fclose(ficheiro);
}

/*
 * Liberta toda a memoria alocada dinamicamente pelo Grafo (vertices e arestas).
 * Crucial para evitar penalizacoes por Memory Leaks.
 */
void liberar_rede(RedeDeAgua* rede)
{
    int i = 0;
    if (rede == NULL) return;

    // Percorre cada vertice
    for (i = 0; i < rede->num_estacoes; i++)
    {
        // Liberta a lista encadeada (canos) atrelada ao vertice
        Cano* atual = rede->estacoes[i]->primeiro_cano;
        while (atual != NULL)
        {
            Cano* temp = atual;
            atual = atual->proximo;
            free(temp);
        }
        // Liberta a estrutura do vertice em si
        free(rede->estacoes[i]);
    }
    // Liberta a base do Grafo
    free(rede);
}
