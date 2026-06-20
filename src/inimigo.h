#ifndef PROG2TRABALHO_INIMIGO_H
#define PROG2TRABALHO_INIMIGO_H

#include <allegro5/allegro.h>
#include <stdbool.h>
#include "player.h"

#define NUM_INIMIGOS 2

typedef enum {
    ESTADO_IDLE,
    ESTADO_WALK,
    ESTADO_HURT,
    ESTADO_DIE,
    ESTADO_ATTACK // NOVO ESTADO
} EstadoInimigo;

typedef struct {
    float x, y;
    int largura_frame, altura_frame;
    int hp;
    bool ativo;
    float velocidade;
    int direcao;
    float frame_atual;
    EstadoInimigo estado;
} Inimigo;

void inicializar_inimigos(Inimigo inimigos[], int quantidade);
void atualizar_inimigos(Inimigo inimigos[], int quantidade, Personagem *player);
void desenhar_inimigos(Inimigo inimigos[], int quantidade,
                       ALLEGRO_BITMAP *sp_idle, ALLEGRO_BITMAP *sp_walk,
                       ALLEGRO_BITMAP *sp_hurt, ALLEGRO_BITMAP *sp_die,
                       ALLEGRO_BITMAP *sp_attack); // NOVO PARÂMETRO

#endif // PROG2TRABALHO_INIMIGO_H