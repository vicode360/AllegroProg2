
#ifndef PROG2TRABALHO_MAPA_H
#define PROG2TRABALHO_MAPA_H

#include <allegro5/allegro.h>
#include "inimigo.h"
#include "player.h"

#define TAM_TILE 64
#define LINHAS_MAPA 8
#define COLUNAS_MAPA 8
#define COLS_TILESET 4

typedef struct MapaNode {
    char arquivo[50];
    int id;
    struct MapaNode *prox;
    struct MapaNode *ant;
} MapaNode;

extern MapaNode *mapa_atual_node;
extern MapaNode *cabeca_lista;
extern int mapa[LINHAS_MAPA][COLUNAS_MAPA];
extern int mapa_id;

void inicializar_lista_mapas(void);
void destruir_lista_mapas(void);
void carregar_mapa_node(MapaNode *no);
void desenhar_mapa(ALLEGRO_BITMAP *tileset);
void mapa_atualizar_bordas(Personagem *p, int w, int h, Inimigo inimigos[]);
void mapa_tentar_entrar_casa(Personagem *p);
void mapa_tentar_sair_casa(Personagem *p);

#endif
