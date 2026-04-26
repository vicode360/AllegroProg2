#include <allegro5/allegro.h>
#include "player.h"


void inicializar_player_sheet(Personagem *p, float x_inicial, float y_inicial, const char* caminho_imagem) {
    p->x = x_inicial;
    p->y = y_inicial;
    p->largura_frame = 0;
    p->altura_frame = 0;
    p->direcao = 0;
    p->frame_atual = 0;

    p->usa_sprite_unico = true;
    p->sprite = al_load_bitmap(caminho_imagem);
}

void inicializar_player_sprites(Personagem *p, float x_inicial, float y_inicial, const char* caminhos[4]) {
    p->x = x_inicial;
    p->y = y_inicial;
    p->largura_frame = 0;
    p->altura_frame = 0;
    p->direcao = 0;
    p->frame_atual = 0;

    p->usa_sprite_unico = false;
    p->sprite = NULL;

    for (int i = 0; i < 4; i++) {
        p->sprites[i] = al_load_bitmap(caminhos[i]);
    }
}

void desenhar_personagem(Personagem p) {
    int corte_x = p.frame_atual * p.largura_frame;

    if (p.usa_sprite_unico) {
        int corte_y = p.direcao * p.altura_frame;
        // aq tu pode mudar o tamanho do sprite
        al_draw_tinted_scaled_rotated_bitmap_region(p.sprite, corte_x, corte_y, p.largura_frame, p.altura_frame, al_map_rgb(255, 255, 255), 1, 1, p.x, p.y, 0.2, 0.2, 0, 0);
    }else {
        al_draw_tinted_scaled_rotated_bitmap_region(p.sprites[p.direcao], corte_x, 0, p.largura_frame, p.altura_frame, al_map_rgb(255, 255, 255), 1, 1, p.x, p.y, 1.25, 1.25, 0, 0);
    }
}

void destruir_personagem(Personagem *p) {
    if (p->sprite != NULL) {
        al_destroy_bitmap(p->sprite);
    }
    for (int i = 0; i < 4; i++) {
        if (p->sprites[i] != NULL) {
            al_destroy_bitmap(p->sprites[i]);
        }
    }
}
