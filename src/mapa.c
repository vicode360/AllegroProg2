#include "mapa.h"

#include <stdio.h>
#include <string.h>

MapaNode* mapa_atual_node = NULL;
MapaNode* cabeca_lista = NULL;
int mapa[LINHAS_MAPA][COLUNAS_MAPA];

// Função auxiliar de alocação do Nó
MapaNode* criar_no_mapa(const char* arquivo, int id) {
    MapaNode* novo = (MapaNode*)malloc(sizeof(MapaNode));
    strcpy(novo->arquivo, arquivo);
    novo->id = id;
    novo->prox = NULL;
    novo->ant = NULL;
    return novo;
}

void inicializar_lista_mapas() {
    // Instancia os 5 mapas do Overworld
    MapaNode* m1 = criar_no_mapa("assets/mapas/mapa01.txt", 1);
    MapaNode* m2 = criar_no_mapa("assets/mapas/mapa02.txt", 2);
    MapaNode* m3 = criar_no_mapa("assets/mapas/mapa04.txt", 4);
    MapaNode* m4 = criar_no_mapa("assets/mapas/mapa05.txt", 5);
    MapaNode* m5 = criar_no_mapa("assets/mapas/mapa06.txt", 6);

    // Conexão Duplamente Encadeada (Esquerda <-> Direita)
    m1->prox = m2; m2->ant = m1;
    m2->prox = m3; m3->ant = m2;
    m3->prox = m4; m4->ant = m3;
    m4->prox = m5; m5->ant = m4;

    cabeca_lista = m1;
    mapa_atual_node = cabeca_lista;
    carregar_mapa_node(mapa_atual_node);
}

void destruir_lista_mapas() {
    MapaNode* atual = cabeca_lista;
    while(atual != NULL) {
        MapaNode* prox = atual->prox;
        free(atual);
        atual = prox;
    }
}

void carregar_mapa_node(MapaNode* no) {
    if (!no) return;

    FILE *arquivo = fopen(no->arquivo, "r");
    if (arquivo == NULL) {
        printf("ERRO DE I/O: Nao foi possivel ler o arquivo %s\n", no->arquivo);
        return;
    }

    for (int i = 0; i < LINHAS_MAPA; i++) {
        for (int j = 0; j < COLUNAS_MAPA; j++) {
            fscanf(arquivo, "%d", &mapa[i][j]);
        }
    }
    fclose(arquivo);
}
void desenhar_mapa(ALLEGRO_BITMAP* tileset) {
    for (int i = 0; i < LINHAS_MAPA; i++) {
        for (int j = 0; j < COLUNAS_MAPA; j++) {
            int tipo_tile = mapa[i][j];
            if (tipo_tile != 0) {
                int indice = tipo_tile - 1;

                int pos_x_no_tileset = (indice % COLS_TILESET) *  TAM_TILE;
                int pos_y_no_tileset = (indice / COLS_TILESET) *  TAM_TILE;

                int pos_x_na_tela = j * TAM_TILE;
                int pos_y_na_tela = i * TAM_TILE;

                al_draw_bitmap_region(tileset, pos_x_no_tileset, pos_y_no_tileset, TAM_TILE, TAM_TILE, pos_x_na_tela, pos_y_na_tela, 0 );
            }
        }
    }
}
