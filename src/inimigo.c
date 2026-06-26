#include "inimigo.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

int kills_no_level = 0;
int num_inimigos_atual = 0;
bool game_over = false;
bool escudo_ativo = false;

static void inimigo_init(Inimigo *e, float x, float y, int hp, float vel,
                         bool is_boss) {
    e->x = x;
    e->y = y;
    e->largura_frame = 90;
    e->altura_frame = 64;
    e->hp = hp;
    e->ativo = true;
    e->velocidade = vel;
    e->direcao = 3;
    e->frame_atual = 0.f;
    e->estado = ESTADO_IDLE;
    e->is_boss = is_boss;
    e->congelado = false;
    e->timer_congelado = 0;
}

void spawn_inimigos_level(Inimigo inimigos[], int level) {
    kills_no_level = 0;
    for (int i = 0; i < MAX_INIMIGOS; i++) inimigos[i].ativo = false;

    int qtd = 0;
    bool boss = false;
    switch (level) {
        case 1: qtd = 5;  break;
        case 2: qtd = 10; break;
        case 3: qtd = 15; break;
        case 4: qtd = 15; boss = true; break;
        default: break;
    }
    num_inimigos_atual = qtd + (boss ? 1 : 0);

    for (int i = 0; i < qtd; i++) {
        float sx = 0.f, sy = 0.f;
        for (int tentativa = 0; tentativa < 20; tentativa++) {
            sx = 130.f + (rand() % 320);
            sy = 50.f + (rand() % 350);
            float dx = sx - 100.f, dy = sy - 100.f;
            if (dx * dx + dy * dy > 160.f * 160.f) break;
        }
        inimigo_init(&inimigos[i], sx, sy, 30, 0.4f + level * 0.1f, false);
    }

    if (boss) inimigo_init(&inimigos[qtd], 250.f, 200.f, 90, 0.3f, true);
}

void atualizar_inimigos(Inimigo inimigos[], int qtd, Personagem *player) {
    for (int i = 0; i < qtd; i++) {
        if (!inimigos[i].ativo) continue;

        if (inimigos[i].congelado) {
            inimigos[i].timer_congelado--;
            if (inimigos[i].timer_congelado <= 0) inimigos[i].congelado = false;
            continue;
        }

        if (inimigos[i].estado == ESTADO_DIE) {
            inimigos[i].frame_atual += 0.3f;
            if (inimigos[i].frame_atual >= 13.f) {
                inimigos[i].ativo = false;
                kills_no_level++;
            }
            continue;
        }
        if (inimigos[i].estado == ESTADO_HURT) {
            inimigos[i].frame_atual += 0.3f;
            if (inimigos[i].frame_atual >= 4.f) inimigos[i].estado = ESTADO_IDLE;
            continue;
        }
        if (inimigos[i].estado == ESTADO_ATTACK) {
            inimigos[i].frame_atual += 0.3f;
            if (inimigos[i].frame_atual >= 11.f) inimigos[i].estado = ESTADO_IDLE;
            continue;
        }

        float dx = player->x - inimigos[i].x;
        float dy = player->y - inimigos[i].y;
        float dist = sqrtf(dx * dx + dy * dy);

        if (dist < 45.f) {
            inimigos[i].estado = ESTADO_ATTACK;
            inimigos[i].frame_atual = 0.f;
            if (escudo_ativo) {
                escudo_ativo = false;
                player->x = 100;
                player->y = 100;
            } else {
                game_over = true;
            }
            continue;
        }

        if (dist > 0 && dist < 350.f) {
            inimigos[i].estado = ESTADO_WALK;
            inimigos[i].x += (dx / dist) * inimigos[i].velocidade;
            inimigos[i].y += (dy / dist) * inimigos[i].velocidade;
            inimigos[i].direcao = (dx < 0) ? 2 : 3;
            inimigos[i].frame_atual += 0.3f;
            if (inimigos[i].frame_atual >= 10.f) inimigos[i].frame_atual -= 10.f;
        } else {
            inimigos[i].estado = ESTADO_IDLE;
            inimigos[i].frame_atual += 0.2f;
            if (inimigos[i].frame_atual >= 8.f) inimigos[i].frame_atual -= 8.f;
        }
    }
}

void desenhar_inimigos(Inimigo inimigos[], int qtd,
                       ALLEGRO_BITMAP *idle, ALLEGRO_BITMAP *walk,
                       ALLEGRO_BITMAP *hurt, ALLEGRO_BITMAP *die,
                       ALLEGRO_BITMAP *attack) {
    for (int i = 0; i < qtd; i++) {
        if (!inimigos[i].ativo) continue;

        ALLEGRO_BITMAP *sp = idle;
        int max_f = 8;
        switch (inimigos[i].estado) {
            case ESTADO_WALK:   sp = walk;   max_f = 10; break;
            case ESTADO_HURT:   sp = hurt;   max_f = 4;  break;
            case ESTADO_DIE:    sp = die;    max_f = 13; break;
            case ESTADO_ATTACK: sp = attack; max_f = 11; break;
            default: break;
        }
        if (!sp) continue;

        int f = (int)inimigos[i].frame_atual;
        if (f >= max_f) f = max_f - 1;
        float esc = inimigos[i].is_boss ? 2.f : 1.f;
        int flag = (inimigos[i].direcao == 2) ? ALLEGRO_FLIP_HORIZONTAL : 0;
        ALLEGRO_COLOR tint = inimigos[i].congelado
            ? al_map_rgba(140, 200, 255, 255)
            : al_map_rgb(255, 255, 255);

        al_draw_tinted_scaled_rotated_bitmap_region(sp,
            f * inimigos[i].largura_frame, 0,
            inimigos[i].largura_frame, inimigos[i].altura_frame,
            tint, 0, 0,
            inimigos[i].x, inimigos[i].y, esc, esc, 0, flag);
    }
}
