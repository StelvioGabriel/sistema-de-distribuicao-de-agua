#include <stdio.h>
#include <stdlib.h>
#include "grafo.h"

int main() {
    RedeDeAgua* epal_luanda = inicializar_rede();


    int eta_ls = adicionar_estacao(epal_luanda, "ETA Luanda Sudeste", "ETA");
    int cd_sapu = adicionar_estacao(epal_luanda, "CD Sapu", "CD");
    int cd_morro = adicionar_estacao(epal_luanda, "CD Morro Bento", "CD");
    int bairro_fut = adicionar_estacao(epal_luanda, "Bairro Futungo", "Bairro");
    int bairro_gam = adicionar_estacao(epal_luanda, "Bairro Gamek", "Bairro");

    adicionar_cano(epal_luanda, eta_ls, cd_sapu, 90000);    
    adicionar_cano(epal_luanda, cd_sapu, cd_morro, 50000);  
    adicionar_cano(epal_luanda, cd_morro, bairro_fut, 15000); 
    

    printf("Teste \n");

    
    encontrar_rota_agua(epal_luanda, eta_ls, bairro_fut);

    liberar_rede(epal_luanda);

    return 0;
}