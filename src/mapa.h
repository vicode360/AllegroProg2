
#ifndef PROG2TRABALHO_MAPA_H
#define PROG2TRABALHO_MAPA_H

#include <allegro5/allegro.h>

#define TAM_TILE 64
#define LINHAS_MAPA 8
#define COLUNAS_MAPA 8
#define COLS_TILESET 4

typedef struct MapaNode {
    char arquivo[50];
    int id; // 1=Casa Externa, 3=Interior, 2,4,5,6=Overworld
    struct MapaNode* prox;
    struct MapaNode* ant;
} MapaNode;

extern MapaNode* mapa_atual_node;
extern MapaNode* cabeca_lista;
extern int mapa[LINHAS_MAPA][COLUNAS_MAPA];


void inicializar_lista_mapas();
void destruir_lista_mapas();
void carregar_mapa_node(MapaNode* no);
void desenhar_mapa(ALLEGRO_BITMAP* tileset);

#endif //PROG2TRABALHO_MAPA_H
