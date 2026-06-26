#ifndef PROG2TRABALHO_INIMIGO_H
#define PROG2TRABALHO_INIMIGO_H

#include <allegro5/allegro.h>
#include <stdbool.h>
#include "player.h"

#define MAX_INIMIGOS 16

typedef enum { ESTADO_IDLE, ESTADO_WALK, ESTADO_HURT, ESTADO_DIE, ESTADO_ATTACK } EstadoInimigo;

typedef struct {
    float x, y;
    int largura_frame, altura_frame;
    int hp;
    bool ativo, is_boss;
    float velocidade, frame_atual;
    int direcao;
    EstadoInimigo estado;
} Inimigo;

extern int kills_no_level;
extern int num_inimigos_atual;
extern bool game_over;
extern bool escudo_ativo;

void spawn_inimigos_level(Inimigo inimigos[], int level);
void atualizar_inimigos(Inimigo inimigos[], int qtd, Personagem *player);
void desenhar_inimigos(Inimigo inimigos[], int qtd,
                       ALLEGRO_BITMAP *idle, ALLEGRO_BITMAP *walk,
                       ALLEGRO_BITMAP *hurt, ALLEGRO_BITMAP *die,
                       ALLEGRO_BITMAP *attack);

#endif
