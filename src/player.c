#include <allegro5/allegro.h>
#include "player.h"

void inicializar_player_sprites(Personagem *p, float x, float y, const char *caminhos[4]) {
    p->x = x;
    p->y = y;
    p->largura_frame = 0;
    p->altura_frame = 0;
    p->direcao = 0;
    p->frame_atual = 0;
    p->atacando = false;
    p->timer_ataque = 0;
    p->slash_aplicado = false;
    for (int i = 0; i < 4; i++)
        p->sprites[i] = al_load_bitmap(caminhos[i]);
}

void desenhar_personagem(Personagem p) {
    int cx = p.frame_atual * p.largura_frame;
    al_draw_tinted_scaled_rotated_bitmap_region(
        p.sprites[p.direcao], cx, 0, p.largura_frame, p.altura_frame,
        al_map_rgb(255, 255, 255), 1, 1, p.x, p.y, 1.25, 1.25, 0, 0);
}

void destruir_personagem(Personagem *p) {
    for (int i = 0; i < 4; i++)
        if (p->sprites[i]) al_destroy_bitmap(p->sprites[i]);
}
