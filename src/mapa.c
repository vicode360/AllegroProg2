#include "mapa.h"
#include "nivel.h"
#include "player.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

MapaNode *mapa_atual_node = NULL;
MapaNode *cabeca_lista = NULL;
int mapa[LINHAS_MAPA][COLUNAS_MAPA];
int mapa_id = 1;

static MapaNode *criar_no(const char *arquivo, int id) {
    MapaNode *n = (MapaNode *)malloc(sizeof(MapaNode));
    strcpy(n->arquivo, arquivo);
    n->id = id;
    n->prox = n->ant = NULL;
    return n;
}

void inicializar_lista_mapas(void) {
    MapaNode *m1 = criar_no("assets/mapas/mapa01.txt", 1);
    MapaNode *m2 = criar_no("assets/mapas/mapa02.txt", 2);
    MapaNode *m4 = criar_no("assets/mapas/mapa04.txt", 4);
    MapaNode *m5 = criar_no("assets/mapas/mapa05.txt", 5);
    MapaNode *m6 = criar_no("assets/mapas/mapa06.txt", 6);

    m1->prox = m2; m2->ant = m1;
    m2->prox = m4; m4->ant = m2;
    m4->prox = m5; m5->ant = m4;
    m5->prox = m6; m6->ant = m5;

    cabeca_lista = mapa_atual_node = m1;
    mapa_id = 1;
    carregar_mapa_node(m1);
}

void destruir_lista_mapas(void) {
    for (MapaNode *atual = cabeca_lista; atual; ) {
        MapaNode *prox = atual->prox;
        free(atual);
        atual = prox;
    }
}

void carregar_mapa_node(MapaNode *no) {
    if (!no) return;
    FILE *f = fopen(no->arquivo, "r");
    if (!f) {
        printf("ERRO: nao foi possivel ler %s\n", no->arquivo);
        return;
    }
    for (int i = 0; i < LINHAS_MAPA; i++)
        for (int j = 0; j < COLUNAS_MAPA; j++)
            fscanf(f, "%d", &mapa[i][j]);
    fclose(f);
}

void desenhar_mapa(ALLEGRO_BITMAP *tileset) {
    for (int i = 0; i < LINHAS_MAPA; i++) {
        for (int j = 0; j < COLUNAS_MAPA; j++) {
            int tile = mapa[i][j];
            if (!tile) continue;
            int idx = tile - 1;
            al_draw_bitmap_region(tileset,
                (idx % COLS_TILESET) * TAM_TILE, (idx / COLS_TILESET) * TAM_TILE,
                TAM_TILE, TAM_TILE, j * TAM_TILE, i * TAM_TILE, 0);
        }
    }
}

static void carregar_interior(void) {
    FILE *f = fopen("assets/mapas/mapa03.txt", "r");
    if (f) {
        for (int i = 0; i < LINHAS_MAPA; i++)
            for (int j = 0; j < COLUNAS_MAPA; j++)
                fscanf(f, "%d", &mapa[i][j]);
        fclose(f);
    }
    mapa_id = 3;
}

void mapa_atualizar_bordas(Personagem *p, int w, int h, Inimigo inimigos[]) {
    if (mapa_id == 3) return;

    if (p->y > h - 60) p->y = h - 60;
    if (p->y < -20) p->y = -20;

    if (p->x > w - 52) {
        if (mapa_atual_node->prox && nivel_completo(inimigos)) {
            mapa_atual_node = mapa_atual_node->prox;
            mapa_id = mapa_atual_node->id;
            carregar_mapa_node(mapa_atual_node);
            p->x = -40;
            nivel_avancar(inimigos);
        } else {
            p->x = w - 52;
        }
    } else if (p->x < -50) {
        p->x = -50;
    }
}

void mapa_tentar_entrar_casa(Personagem *p) {
    if (mapa_id != 1) return;
    if (p->x <= 250 || p->x >= 285 || p->y <= 140 || p->y >= 200) return;
    carregar_interior();
    p->x = 200;
    p->y = 300;
}

void mapa_tentar_sair_casa(Personagem *p) {
    if (mapa_id != 3) return;
    if (p->x <= 170 || p->x >= 210 || p->y <= 300) return;
    mapa_atual_node = cabeca_lista;
    mapa_id = mapa_atual_node->id;
    carregar_mapa_node(mapa_atual_node);
    p->x = 268;
    p->y = 220;
}
