#define MAX_ESTACOES 50

typedef struct Cano
{
    int id_destino;
    int capacidade;
    struct Cano* proximo;
} Cano;

typedef struct Estacao
{
    char nome[100];
    char tipo[50];
    Cano* primeiro_cano;
} Estacao;

typedef struct RedeDeAgua
{
    int num_estacoes;
    Estacao* estacoes[MAX_ESTACOES];
} RedeDeAgua;

RedeDeAgua* inicializar_rede();
int adicionar_estacao(RedeDeAgua* rede, const char* nome, const char* tipo);
void adicionar_cano(RedeDeAgua* rede, int id_origem, int id_destino, int capacidade);
void encontrar_rota_agua(RedeDeAgua* rede, int id_origem, int id_destino);
int guardar_dados_txt(RedeDeAgua* rede, const char* nome_arquivo);
RedeDeAgua* carregar_dados_txt(const char* nome_arquivo);
void ler_dados_txt(const char* nome_arquivo);
void liberar_rede(RedeDeAgua* rede);
