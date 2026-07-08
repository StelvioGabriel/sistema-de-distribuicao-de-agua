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

static void ler_texto(const char* prompt, char* destino, size_t tamanho)
{
    if (destino == NULL || tamanho == 0) {
        return;
    }

    printf("%s", prompt);
    if (fgets(destino, (int)tamanho, stdin) == NULL) {
        destino[0] = '\0';
        return;
    }

    remover_quebra_linha(destino);
}

static int ler_inteiro(const char* prompt, int* valor)
{
    char linha[64];
    char* fim = NULL;
    long numero = 0;

    while (1) {
        printf("%s", prompt);
        if (fgets(linha, sizeof(linha), stdin) == NULL) {
            return 0;
        }

        numero = strtol(linha, &fim, 10);
        while (fim != NULL && (*fim == ' ' || *fim == '\t')) {
            fim++;
        }

        if (fim != linha && fim != NULL && (*fim == '\n' || *fim == '\0')) {
            *valor = (int)numero;
            return 1;
        }

        printf("Entrada invalida. Introduza um numero inteiro.\n");
    }
}

static void listar_estacoes(const RedeDeAgua* rede)
{
    int i = 0;

    if (rede == NULL || rede->num_estacoes == 0) {
        printf("Nenhuma estacao cadastrada na rede.\n");
        return;
    }

    printf("\nEstacoes disponiveis:\n");
    for (i = 0; i < rede->num_estacoes; i++) {
        printf("[%d] %s (%s)\n", i, rede->estacoes[i]->nome, rede->estacoes[i]->tipo);
    }
    printf("\n");
}

static void mostrar_menu(void)
{
    printf("===== MENU DA REDE DE DISTRIBUICAO DE AGUA =====\n");
    printf("1. Adicionar estacao\n");
    printf("2. Adicionar cano\n");
    printf("3. Listar estacoes\n");
    printf("4. Guardar dados no ficheiro\n");
    printf("5. Carregar rede a partir do ficheiro\n");
    printf("6. Encontrar rota de agua\n");
    printf("7. Remover cano\n");
    printf("8. Remover estacao\n");
    printf("9. Listar todas as ligacoes (canos)\n");
    printf("0. Sair\n");
}

int main(void)
{
    const char* arquivo_dados = "dados_rede.txt";
    RedeDeAgua* rede = inicializar_rede();
    int opcao = -1;

    if (rede == NULL) {
        return 1;
    }

    do {
        mostrar_menu();
        if (!ler_inteiro("Escolha uma opcao: ", &opcao)) {
            break;
        }

        printf("\n");

        switch (opcao) {
            case 1: {
                char nome[100];
                char tipo[50];
                int id = 0;

                ler_texto("Nome da estacao: ", nome, sizeof(nome));
                ler_texto("Tipo da estacao: ", tipo, sizeof(tipo));

                id = adicionar_estacao(rede, nome, tipo);
                if (id == -1) {
                    printf("Nao foi possivel adicionar a estacao.\n\n");
                } else {
                    printf("Estacao adicionada com sucesso. ID atribuido: %d\n\n", id);
                }
                break;
            }
            case 2: {
                int origem = 0;
                int destino = 0;
                int capacidade = 0;

                listar_estacoes(rede);
                if (rede->num_estacoes < 2) {
                    printf("Adicione pelo menos duas estacoes antes de criar um cano.\n\n");
                    break;
                }

                if (!ler_inteiro("ID da estacao de origem: ", &origem) ||
                    !ler_inteiro("ID da estacao de destino: ", &destino) ||
                    !ler_inteiro("Capacidade do cano: ", &capacidade)) {
                    printf("Operacao cancelada.\n\n");
                    break;
                }

                if (origem < 0 || destino < 0 || origem >= rede->num_estacoes ||
                    destino >= rede->num_estacoes || capacidade <= 0) {
                    printf("Dados invalidos para criar o cano.\n\n");
                    break;
                }

                adicionar_cano(rede, origem, destino, capacidade);
                printf("Cano registado com sucesso.\n\n");
                break;
            }
            case 3:
                listar_estacoes(rede);
                break;
            case 4:
                if (guardar_dados_txt(rede, arquivo_dados)) {
                    printf("Dados guardados com sucesso em %s.\n\n", arquivo_dados);
                } else {
                    printf("Nao foi possivel guardar os dados.\n\n");
                }
                break;
            case 5: {
                RedeDeAgua* rede_carregada = carregar_dados_txt(arquivo_dados);
                if (rede_carregada == NULL) {
                    printf("Falha ao carregar os dados do ficheiro.\n\n");
                    break;
                }

                liberar_rede(rede);
                rede = rede_carregada;
                printf("Rede carregada com sucesso a partir do ficheiro.\n\n");
                break;
            }
            case 6: {
                int origem = 0;
                int destino = 0;

                listar_estacoes(rede);
                if (rede->num_estacoes == 0) {
                    break;
                }

                if (!ler_inteiro("ID da estacao de origem: ", &origem) ||
                    !ler_inteiro("ID da estacao de destino: ", &destino)) {
                    printf("Operacao cancelada.\n\n");
                    break;
                }

                encontrar_rota_agua(rede, origem, destino);
                break;
            }
            case 7: {
                int origem = 0;
                int destino = 0;
                
                listar_estacoes(rede);
                if (rede->num_estacoes < 2) {
                    printf("Adicione estacoes e canos primeiro.\n\n");
                    break;
                }

                if (!ler_inteiro("ID da estacao de origem: ", &origem) ||
                    !ler_inteiro("ID da estacao de destino (cano a remover): ", &destino)) {
                    printf("Operacao cancelada.\n\n");
                    break;
                }

                if (remover_cano(rede, origem, destino)) {
                    printf("Cano removido com sucesso da memoria.\n\n");
                } else {
                    printf("Erro: Nao foi possivel remover o cano. Verifique se a ligacao existe.\n\n");
                }
                break;
            }
            case 8: {
                int id_estacao = 0;
                
                listar_estacoes(rede);
                if (rede->num_estacoes == 0) break;

                if (!ler_inteiro("ID da estacao a remover: ", &id_estacao)) {
                    printf("Operacao cancelada.\n\n");
                    break;
                }

                if (remover_estacao(rede, id_estacao)) {
                    printf("Estacao removida com sucesso. Nota: Os IDs das estacoes podem ter sido reorganizados.\n\n");
                } else {
                    printf("Erro: Nao foi possivel remover a estacao. ID invalido.\n\n");
                }
                break;
            }
            case 9: {
                listar_todas_ligacoes(rede);
                break;
            }
            case 0:
                printf("A encerrar o programa...\n");
                break;
            default:
                printf("Opcao invalida. Tente novamente.\n\n");
                break;
        }
    } while (opcao != 0);

    liberar_rede(rede);
    return 0;
}

