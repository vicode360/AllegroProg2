#include "render.h"
#include "nivel.h"
#include <math.h>
#include <stdio.h>

void render_tela_fim(ALLEGRO_FONT *font, int w, int h, bool vitoria, int objetivos) {
    al_clear_to_color(vitoria ? al_map_rgb(0, 0, 0) : al_map_rgb(30, 0, 0));
    if (vitoria) {
        al_draw_text(font, al_map_rgb(255, 215, 0), w / 2, h / 2 - 30,
                     ALLEGRO_ALIGN_CENTRE, "VOCE GANHOU!");
        char buf[64];
        snprintf(buf, sizeof(buf), "Objetivos concluidos: %d/5", objetivos);
        al_draw_text(font, al_map_rgb(200, 200, 200), w / 2, h / 2,
                     ALLEGRO_ALIGN_CENTRE, buf);
    } else {
        al_draw_text(font, al_map_rgb(255, 50, 50), w / 2, h / 2 - 20,
                     ALLEGRO_ALIGN_CENTRE, "GAME OVER");
    }
    al_draw_text(font, al_map_rgb(200, 200, 200), w / 2, h / 2 + 30,
                 ALLEGRO_ALIGN_CENTRE, "Pressione qualquer tecla para sair");
    al_flip_display();
}

void render_hud(ALLEGRO_FONT *font, int w, bool nivel_pronto) {
    char buf[128];
    if (nivel_atual <= 4) {
        int alvo = nivel_kills_alvo();
        if (nivel_atual == 4 && nivel_boss_ok()) {
            snprintf(buf, sizeof(buf), "Level %d | Kills: %d/%d + Boss [%s]",
                     nivel_atual, kills_no_level, alvo, nivel_boss()->name);
        } else if (nivel_atual == 4) {
            snprintf(buf, sizeof(buf), "Level %d | Kills: %d/%d + Boss",
                     nivel_atual, kills_no_level, alvo);
        } else {
            snprintf(buf, sizeof(buf), "Level %d | Kills: %d/%d",
                     nivel_atual, kills_no_level, alvo);
        }
    } else {
        snprintf(buf, sizeof(buf), "Level %d", nivel_atual);
    }

    al_draw_filled_rectangle(0, 0, w, 40, al_map_rgba(0, 0, 0, 180));
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

    const char *obj_nome = nivel_objetivo_nome();
    if (obj_nome[0]) {
        char obj_buf[96];
        const char *marca;
        ALLEGRO_COLOR cor;
        if (nivel_objetivo_concluido()) { marca = "[v]"; cor = al_map_rgb(0, 255, 0); }
        else if (nivel_objetivo_falhou()) { marca = "[x]"; cor = al_map_rgb(255, 80, 80); }
        else { marca = "[ ]"; cor = al_map_rgb(255, 215, 0); }
        snprintf(obj_buf, sizeof(obj_buf), "Objetivo: %s %s", obj_nome, marca);
        al_draw_text(font, cor, w - 4, 22, ALLEGRO_ALIGN_RIGHT, obj_buf);
    }
}

void render_pickup(ALLEGRO_FONT *font, const PokemonPickup *pk) {
    if (!pk->ativo || !pk->pronto) return;
    float x = pk->x, y = pk->y;
    if (pk->sprite) {
        int sw = al_get_bitmap_width(pk->sprite);
        int sh = al_get_bitmap_height(pk->sprite);
        float s = 52.f / (sw > sh ? sw : sh);
        al_draw_scaled_bitmap(pk->sprite, 0, 0, sw, sh,
                              x - (sw * s) / 2.f, y - (sh * s) / 2.f,
                              sw * s, sh * s, 0);
    } else {
        char inicial[2] = {pk->pokemon.name[0] ? pk->pokemon.name[0] : '?', '\0'};
        al_draw_text(font, al_map_rgb(255, 255, 255), x, y - 7, ALLEGRO_ALIGN_CENTRE, inicial);
    }
    al_draw_text(font, al_map_rgb(255, 215, 0), x, y - 40, ALLEGRO_ALIGN_CENTRE, pk->pokemon.name);
}

void render_pokedex(ALLEGRO_FONT *font, int pagina, int total,
                    const PokemonData *p, ALLEGRO_BITMAP *sprite, bool pronto) {
    const int bx = 10, by = 44, bw = 360, bh = 150;

    al_draw_filled_rectangle(bx, by, bx + bw, by + bh, al_map_rgba(0, 0, 0, 200));
    al_draw_rectangle(bx, by, bx + bw, by + bh, al_map_rgb(255, 215, 0), 1);

    char titulo[80];
    snprintf(titulo, sizeof(titulo), "POKEDEX  (%d/%d)", pagina + 1, total);
    al_draw_text(font, al_map_rgb(255, 215, 0), bx + 8, by + 6, 0, titulo);

    if (!pronto) {
        al_draw_text(font, al_map_rgb(200, 200, 200), bx + 8, by + 60, 0, "Carregando...");
        al_draw_text(font, al_map_rgb(150, 150, 150), bx + 8, by + bh - 18, 0, "[Q] ant  [E] prox");
        return;
    }

    int tx = bx + 8;
    if (sprite) {
        int sw = al_get_bitmap_width(sprite);
        int sh = al_get_bitmap_height(sprite);
        float s = 84.f / (sw > sh ? sw : sh);
        int dw = (int)(sw * s), dh = (int)(sh * s);
        al_draw_scaled_bitmap(sprite, 0, 0, sw, sh,
                              bx + 8, by + 30, dw, dh, 0);
        tx = bx + 8 + dw + 10;
    }

    char l1[80], l2[64], l3[64];
    snprintf(l1, sizeof(l1), "%s [%s]", p->name, p->type);
    snprintf(l2, sizeof(l2), "HP:%d ATK:%d DEF:%d", p->hp, p->attack, p->defense);
    snprintf(l3, sizeof(l3), "SPD:%d SP.ATK:%d SP.DEF:%d", p->speed, p->special_attack, p->special_defense);
    al_draw_text(font, al_map_rgb(255, 255, 255), tx, by + 30, 0, l1);
    al_draw_text(font, al_map_rgb(200, 200, 200), tx, by + 48, 0, l2);
    al_draw_text(font, al_map_rgb(200, 200, 200), tx, by + 64, 0, l3);

    if (p->descricao[0]) {
        const int max_w = bx + bw - tx - 6;
        int dy = by + 86;
        char linha[80];
        int li = 0;
        for (const char *c = p->descricao; *c && dy < by + bh - 22; c++) {
            linha[li++] = *c;
            linha[li] = '\0';
            if (li >= (int)sizeof(linha) - 1 || al_get_text_width(font, linha) > max_w) {
                if (li > 1) { linha[li - 1] = '\0'; c--; }
                al_draw_text(font, al_map_rgb(180, 180, 220), tx, dy, 0, linha);
                dy += 12;
                li = 0;
            }
        }
        if (li > 0 && dy < by + bh - 22) {
            al_draw_text(font, al_map_rgb(180, 180, 220), tx, dy, 0, linha);
        }
    }

    al_draw_text(font, al_map_rgb(150, 150, 150), bx + 8, by + bh - 18, 0, "[Q] ant  [E] prox");
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
