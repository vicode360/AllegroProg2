#include "inimigo.h"
#include <math.h>
#include <stdio.h>

void inicializar_inimigos(Inimigo inimigos[], int quantidade) {
    for (int i = 0; i < quantidade; i++) {
        inimigos[i].x = 100.0f + (i * 250.0f);
        inimigos[i].y = 150.0f + (i * 100.0f);

        inimigos[i].largura_frame = 90;
        inimigos[i].altura_frame = 64;

        inimigos[i].hp = 30;
        inimigos[i].ativo = true;
        inimigos[i].velocidade = 1.2f;
        inimigos[i].direcao = 3;
        inimigos[i].frame_atual = 0.0f;
        inimigos[i].estado = ESTADO_IDLE;
    }
}

void atualizar_inimigos(Inimigo inimigos[], int quantidade, Personagem *player) {
    for (int i = 0; i < quantidade; i++) {
        if (!inimigos[i].ativo) continue;

        // Máquina de Estado 1: Morte
        if (inimigos[i].estado == ESTADO_DIE) {
            inimigos[i].frame_atual += 0.3f;
            if (inimigos[i].frame_atual >= 13.0f) inimigos[i].ativo = false;
            continue;
        }

        // Máquina de Estado 2: Dano
        if (inimigos[i].estado == ESTADO_HURT) {
            inimigos[i].frame_atual += 0.3f;
            if (inimigos[i].frame_atual >= 4.0f) inimigos[i].estado = ESTADO_IDLE;
            continue;
        }

        // Máquina de Estado 3: Ataque (Prioridade sobre caminhada)
        if (inimigos[i].estado == ESTADO_ATTACK) {
            inimigos[i].frame_atual += 0.3f;
            if (inimigos[i].frame_atual >= 11.0f) { // Golem_1_attack.png tem 11 frames
                inimigos[i].estado = ESTADO_IDLE;   // Conclui animação
            }
            continue; // Entidade permanece parada enquanto ataca
        }

        float dx = player->x - inimigos[i].x;
        float dy = player->y - inimigos[i].y;
        float dist = sqrt(dx*dx + dy*dy);

        // Gatilho de Toque e Morte Instantânea (Hitbox de 45px de raio)
        if (dist < 45.0f) {
            inimigos[i].estado = ESTADO_ATTACK;
            inimigos[i].frame_atual = 0.0f;

            // Lógica de "Morte Instantânea" (Teleporte para Respawn / Perda de Estado)
            printf("GAME OVER: O Golem aniquilou o jogador!\n");
            player->x = 268; // Coordenada de Respawn no Mapa 1
            player->y = 220;
            continue;
        }

        // Máquina de Estado 4: Perseguição
        if (dist > 0 && dist < 350.0f) {
            inimigos[i].estado = ESTADO_WALK;
            inimigos[i].x += (dx / dist) * inimigos[i].velocidade;
            inimigos[i].y += (dy / dist) * inimigos[i].velocidade;

            if (dx < 0) inimigos[i].direcao = 2; // Esquerda
            else inimigos[i].direcao = 3;        // Direita

            inimigos[i].frame_atual += 0.3f;
            if (inimigos[i].frame_atual >= 10.0f) inimigos[i].frame_atual -= 10.0f;

        } else {
            inimigos[i].estado = ESTADO_IDLE;
            inimigos[i].frame_atual += 0.2f;
            if (inimigos[i].frame_atual >= 8.0f) inimigos[i].frame_atual -= 8.0f;
        }
    }
}

void desenhar_inimigos(Inimigo inimigos[], int quantidade,
                       ALLEGRO_BITMAP *sp_idle, ALLEGRO_BITMAP *sp_walk,
                       ALLEGRO_BITMAP *sp_hurt, ALLEGRO_BITMAP *sp_die,
                       ALLEGRO_BITMAP *sp_attack) {

    for (int i = 0; i < quantidade; i++) {
        if (!inimigos[i].ativo) continue;

        ALLEGRO_BITMAP *sprite_atual = NULL;
        int max_frames = 1;

        switch (inimigos[i].estado) {
            case ESTADO_IDLE:   sprite_atual = sp_idle;   max_frames = 8;  break;
            case ESTADO_WALK:   sprite_atual = sp_walk;   max_frames = 10; break;
            case ESTADO_HURT:   sprite_atual = sp_hurt;   max_frames = 4;  break;
            case ESTADO_DIE:    sprite_atual = sp_die;    max_frames = 13; break;
            case ESTADO_ATTACK: sprite_atual = sp_attack; max_frames = 11; break; // Mapeamento
        }

        if (!sprite_atual) continue;

        int frame_int = (int)inimigos[i].frame_atual;
        if (frame_int >= max_frames) frame_int = max_frames - 1;

        int corte_x = frame_int * inimigos[i].largura_frame;
        int corte_y = 0;

        int flag = (inimigos[i].direcao == 2) ? ALLEGRO_FLIP_HORIZONTAL : 0;

        al_draw_tinted_bitmap_region(
            sprite_atual,
            al_map_rgb(255, 255, 255),
            corte_x, corte_y,
            inimigos[i].largura_frame, inimigos[i].altura_frame,
            inimigos[i].x, inimigos[i].y,
            flag
        );
    }
}