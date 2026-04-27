#include "mapa.h"

#include <stdio.h>
#include <string.h>

char mapa_atual[] = "mapa02.txt";
int mapa[LINHAS_MAPA][COLUNAS_MAPA];

/*
if (mudou_de_tela) {
    strcpy (mapa_atual, "mapa02.txt");
}
*/

void carregar_mapa() {
    FILE *arquivo = fopen(mapa_atual, "r");
    if (arquivo == NULL) {
        printf("erro na leitura do arquivo\n");
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
