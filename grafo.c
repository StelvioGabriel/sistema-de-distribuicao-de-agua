#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "grafo.h"

static void remover_quebra_linha(char* texto)
{
    if (texto == NULL) {
        return;
    }

    texto[strcspn(texto, "\r\n")] = '\0';
}

RedeDeAgua* inicializar_rede()
{
	int i = 0;
    RedeDeAgua* rede = (RedeDeAgua*)malloc(sizeof(RedeDeAgua));
    if (rede == NULL) {
        printf("Erro de alocacao de memoria para a rede.\n");
        return NULL;
    }

    rede->num_estacoes = 0;
    for (i = 0; i < MAX_ESTACOES; i++) {
        rede->estacoes[i] = NULL;
    }

    return rede;
}

int adicionar_estacao(RedeDeAgua* rede, const char* nome, const char* tipo)
{
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
    nova->primeiro_cano = NULL;

    int id = rede->num_estacoes;
    rede->estacoes[id] = nova;
    rede->num_estacoes++;
    return id;
}

void adicionar_cano(RedeDeAgua* rede, int id_origem, int id_destino, int capacidade)
{
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

    novo_cano->id_destino = id_destino;
    novo_cano->capacidade = capacidade;
    novo_cano->proximo = rede->estacoes[id_origem]->primeiro_cano;
    rede->estacoes[id_origem]->primeiro_cano = novo_cano;
}

void encontrar_rota_agua(RedeDeAgua* rede, int id_origem, int id_destino)
{
    if (rede == NULL || id_origem < 0 || id_destino < 0 ||
        id_origem >= rede->num_estacoes || id_destino >= rede->num_estacoes) {
        printf("Erro: Nao foi possivel procurar a rota.\n");
        return;
    }

    int visitados[MAX_ESTACOES] = {0};
    int estacao_anterior[MAX_ESTACOES], i = 0;

    for (i = 0; i < MAX_ESTACOES; i++)
    {
        estacao_anterior[i] = -1;
    }

    int fila[MAX_ESTACOES];
    int inicio = 0, fim = 0;

    fila[fim++] = id_origem;
    visitados[id_origem] = 1;
    int achou_destino = 0;

    while (inicio < fim) {
        int atual = fila[inicio++];

        if (atual == id_destino)
        {
            achou_destino = 1;
            break;
        }

        Cano* cano_atual = rede->estacoes[atual]->primeiro_cano;
        while (cano_atual != NULL)
        {
            int vizinho = cano_atual->id_destino;
            if (visitados[vizinho] == 0)
            {
                visitados[vizinho] = 1;
                estacao_anterior[vizinho] = atual;
                fila[fim++] = vizinho;
            }
            cano_atual = cano_atual->proximo;
        }
    }

    if (achou_destino)
    {
        printf("\nSUCESSO: Rota encontrada de [%s] para [%s]:\n",
               rede->estacoes[id_origem]->nome, rede->estacoes[id_destino]->nome);

        int caminho[MAX_ESTACOES];
        int tamanho_caminho = 0;
        int rastreador = id_destino, i = 0;

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
        printf("\nFALHA: Nao existe conexao.\n");
    }
}

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
    for (i = 0; i < rede->num_estacoes; i++) {
        Cano* cano = rede->estacoes[i]->primeiro_cano;
        while (cano != NULL) {
            total_canos++;
            cano = cano->proximo;
        }
    }

    fprintf(ficheiro, "%d\n", rede->num_estacoes);
    for (i = 0; i < rede->num_estacoes; i++) {
        fprintf(ficheiro, "%s;%s\n", rede->estacoes[i]->nome, rede->estacoes[i]->tipo);
    }

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

        *separador = '\0';
        char* nome = linha;
        char* tipo = separador + 1;

        if (adicionar_estacao(rede, nome, tipo) == -1) {
            liberar_rede(rede);
            fclose(ficheiro);
            return NULL;
        }
    }

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

    for (i = 0; i < total_canos; i++) {
        if (fgets(linha, sizeof(linha), ficheiro) == NULL) {
            printf("Erro: Dados de cano incompletos.\n");
            liberar_rede(rede);
            fclose(ficheiro);
            return NULL;
        }

        int id_origem = 0;
        int id_destino = 0;
        int capacidade = 0;
        if (sscanf(linha, "%d;%d;%d", &id_origem, &id_destino, &capacidade) != 3) {
            printf("Erro: Linha de cano invalida.\n");
            liberar_rede(rede);
            fclose(ficheiro);
            return NULL;
        }

        adicionar_cano(rede, id_origem, id_destino, capacidade);
    }

    fclose(ficheiro);
    return rede;
}

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
    printf("Conteudo do ficheiro %s:\n\n", nome_arquivo);
    while (fgets(linha, sizeof(linha), ficheiro) != NULL) {
        printf("%s", linha);
    }
    printf("\n");

    fclose(ficheiro);
}

void liberar_rede(RedeDeAgua* rede)
{
	int i = 0;
	
    if (rede == NULL) return;

    for (i = 0; i < rede->num_estacoes; i++)
    {
        Cano* atual = rede->estacoes[i]->primeiro_cano;
        while (atual != NULL)
        {
            Cano* temp = atual;
            atual = atual->proximo;
            free(temp);
        }
        free(rede->estacoes[i]);
    }
    free(rede);
}
