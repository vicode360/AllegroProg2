
#ifndef PROG2TRABALHO_MAPA_H
#define PROG2TRABALHO_MAPA_H

#include <allegro5/allegro.h>

#define TAM_TILE 64
#define LINHAS_MAPA 8
#define COLUNAS_MAPA 8
#define COLS_TILESET 4

void carregar_mapa(const char* caminho_mapa);
void desenhar_mapa(ALLEGRO_BITMAP* tileset);

#endif //PROG2TRABALHO_MAPA_H
