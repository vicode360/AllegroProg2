#include "render.h"
#include "nivel.h"
#include <stdio.h>

void render_tela_fim(ALLEGRO_FONT *font, int w, int h, bool vitoria) {
    al_clear_to_color(vitoria ? al_map_rgb(0, 0, 0) : al_map_rgb(30, 0, 0));
    if (vitoria) {
        al_draw_text(font, al_map_rgb(255, 215, 0), w / 2, h / 2 - 20,
                     ALLEGRO_ALIGN_CENTRE, "VOCE GANHOU!");
    } else {
        al_draw_text(font, al_map_rgb(255, 50, 50), w / 2, h / 2 - 20,
                     ALLEGRO_ALIGN_CENTRE, "GAME OVER");
    }
    al_draw_text(font, al_map_rgb(200, 200, 200), w / 2, h / 2 + 10,
                 ALLEGRO_ALIGN_CENTRE, "Pressione qualquer tecla para sair");
    al_flip_display();
}

void render_hud(ALLEGRO_FONT *font, int w, bool nivel_pronto) {
    char buf[128];
    if (nivel_atual <= 4) {
        static const int alvos[] = {0, 5, 10, 15, 15};
        if (nivel_atual == 4 && nivel_boss_ok()) {
            snprintf(buf, sizeof(buf), "Level %d | Kills: %d/%d + Boss [%s]",
                     nivel_atual, kills_no_level, alvos[nivel_atual], nivel_boss()->name);
        } else if (nivel_atual == 4) {
            snprintf(buf, sizeof(buf), "Level %d | Kills: %d/%d + Boss",
                     nivel_atual, kills_no_level, alvos[nivel_atual]);
        } else {
            snprintf(buf, sizeof(buf), "Level %d | Kills: %d/%d",
                     nivel_atual, kills_no_level, alvos[nivel_atual]);
        }
    } else {
        snprintf(buf, sizeof(buf), "Level %d", nivel_atual);
    }

    al_draw_filled_rectangle(0, 0, w, 28, al_map_rgba(0, 0, 0, 180));
    al_draw_text(font, al_map_rgb(255, 255, 255), 4, 2, 0, buf);

    if (nivel_pokemon_ok()) {
        al_draw_text(font, al_map_rgb(100, 255, 100), 4, 14, 0, nivel_pokemon_hud());
    }
    if (escudo_ativo) {
        al_draw_text(font, al_map_rgb(0, 200, 255), w / 2, 14,
                     ALLEGRO_ALIGN_CENTRE, "[ESCUDO ATIVO]");
    }
    if (nivel_pronto && nivel_atual < 5) {
        al_draw_text(font, al_map_rgb(0, 255, 0), w - 4, 2,
                     ALLEGRO_ALIGN_RIGHT, ">> AVANCE >>");
    }
}

void render_pokedex(ALLEGRO_FONT *font, const PokemonData *p) {
    char l1[64], l2[64], l3[64];
    snprintf(l1, sizeof(l1), "POKEDEX: %s [%s]", p->name, p->type);
    snprintf(l2, sizeof(l2), "HP:%d ATK:%d DEF:%d", p->hp, p->attack, p->defense);
    snprintf(l3, sizeof(l3), "SPD:%d SP.ATK:%d SP.DEF:%d", p->speed, p->special_attack, p->special_defense);

    al_draw_filled_rectangle(10, 10, 220, 100, al_map_rgba(0, 0, 0, 180));
    al_draw_rectangle(10, 10, 220, 100, al_map_rgb(255, 215, 0), 1);
    al_draw_text(font, al_map_rgb(255, 215, 0), 18, 18, 0, l1);
    al_draw_text(font, al_map_rgb(200, 200, 200), 18, 38, 0, l2);
    al_draw_text(font, al_map_rgb(200, 200, 200), 18, 58, 0, l3);
}

void render_casa(ALLEGRO_BITMAP *casa, bool parte_frente) {
    if (parte_frente) {
        al_draw_tinted_scaled_rotated_bitmap_region(casa, 0, 0, 80, 63,
            al_map_rgb(255, 255, 255), 1, 1, 250, 80, 2, 2, 0, 0);
    } else {
        al_draw_tinted_scaled_rotated_bitmap_region(casa, 0, 63, 80, 34,
            al_map_rgb(255, 255, 255), 1, 1, 250, 206, 2, 2, 0, 0);
    }
}

void render_slash(Personagem player, ALLEGRO_BITMAP *frames[6]) {
    if (!player.atacando) return;

    int frame = 5 - (player.timer_ataque * 6 / 16);
    if (frame < 0) frame = 0;
    if (frame > 5) frame = 5;

    float angulo = 0;
    int flag = 0, ox = 0, oy = 0;
    switch (player.direcao) {
        case 0: angulo = ALLEGRO_PI / 2.f;  oy = 40;  break;
        case 1: angulo = -ALLEGRO_PI / 2.f; oy = -40; break;
        case 2: angulo = 0; flag = ALLEGRO_FLIP_HORIZONTAL; ox = -40; break;
        default: ox = 40; break;
    }

    int sw = al_get_bitmap_width(frames[frame]);
    int sh = al_get_bitmap_height(frames[frame]);
    al_draw_tinted_scaled_rotated_bitmap_region(frames[frame], 0, 0, sw, sh,
        al_map_rgb(255, 255, 255), sw / 2.f, sh / 2.f,
        player.x + 55 + ox, player.y + 50 + oy, 0.2f, 0.2f, angulo, flag);
}
