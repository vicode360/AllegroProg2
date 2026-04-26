#include "mapa.h"


int mapa[LINHAS_MAPA][COLUNAS_MAPA] = {
        {3, 4, 7, 8, 3, 7, 3, 8},
        {11, 11, 12, 11, 12, 11, 12, 11},
        {1, 2, 5, 14, 6, 1, 5, 2},
        {5, 6, 1, 10, 5, 1, 2, 6},
        {5, 6, 1, 14, 12, 16, 2, 6},
        {1, 2, 5, 8, 6, 1, 5, 2},
        {3, 2, 4, 8, 6, 2, 6, 2},
        {3, 4, 7, 8, 3, 7, 3, 8},
    };

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