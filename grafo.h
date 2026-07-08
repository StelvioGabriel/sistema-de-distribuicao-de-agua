#define MAX_ESTACOES 50

typedef struct Cano {
    int id_destino;
    int capacidade;
    struct Cano* proximo;
} Cano;

typedef struct Estacao {
    char nome[100];
    char tipo[50];
    Cano* primeiro_cano;
} Estacao;

typedef struct RedeDeAgua {
    int num_estacoes;
    Estacao* estacoes[MAX_ESTACOES];
} RedeDeAgua;


RedeDeAgua* inicializar_rede();
int adicionar_estacao(RedeDeAgua* rede, const char* nome, const char* tipo);
void adicionar_cano(RedeDeAgua* rede, int id_origem, int id_destino, int capacidade);
void listar_estacoes(const RedeDeAgua* rede);
int guardar_dados_txt(RedeDeAgua* rede, const char* arquivo);
RedeDeAgua* carregar_dados_txt(const char* arquivo);
void encontrar_rota_agua(RedeDeAgua* rede, int id_origem, int id_destino);
int remover_cano(RedeDeAgua* rede, int id_origem, int id_destino);
int remover_estacao(RedeDeAgua* rede, int id_estacao);
void listar_todas_ligacoes(const RedeDeAgua* rede);
void liberar_rede(RedeDeAgua* rede);
