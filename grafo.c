#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "grafo.h"

RedeDeAgua* inicializar_rede()
{
    RedeDeAgua* rede = (RedeDeAgua*)malloc(sizeof(RedeDeAgua));
    if (rede == NULL) {
        printf("Erro de alocacao de memoria para a rede.\n");
        return 0;
    }
    rede->num_estacoes = 0;
    return rede;
}

int adicionar_estacao(RedeDeAgua* rede, const char* nome, const char* tipo)
{
    if (rede->num_estacoes >= MAX_ESTACOES)
	{
        printf("Erro: Limite maximo de estacoes atingido.\n");
        return -1;
    }

    Estacao* nova = (Estacao*)malloc(sizeof(Estacao));
    if (nova == NULL)
	{
        printf("Erro de alocacao de memoria para a estacao.\n");
        return 0;
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
    Cano* novo_cano = (Cano*)malloc(sizeof(Cano));
    if (novo_cano == NULL) {
        printf("Erro de alocacao de memoria\n");
        return 0 ; 
    }
    
    novo_cano->id_destino = id_destino;
    novo_cano->capacidade = capacidade;
    
    novo_cano->proximo = rede->estacoes[id_origem]->primeiro_cano;
    rede->estacoes[id_origem]->primeiro_cano = novo_cano;
}



void encontrar_rota_agua(RedeDeAgua* rede, int id_origem, int id_destino)
{
    int visitados[MAX_ESTACOES] = {0}; 
    int estacao_anterior[MAX_ESTACOES];
    
    for (int i = 0; i < MAX_ESTACOES; i++)
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
        int rastreador = id_destino;

  
        while (rastreador != -1)
		{
            caminho[tamanho_caminho++] = rastreador;
            rastreador = estacao_anterior[rastreador];
        }


        printf("ROTA: ");
        for (int i = tamanho_caminho - 1; i >= 0; i--)
		{
            printf("%s", rede->estacoes[caminho[i]]->nome);
            if (i > 0) printf(" -> ");
        }
        printf("\n\n");

    } else
	    {
        printf("\nFALHA: Nao existe conexao \n"); 
               
        }
}


void liberar_rede(RedeDeAgua* rede)
{
    if (rede == NULL) return;
    
    for (int i = 0; i < rede->num_estacoes; i++)
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