#ifndef PROG2TRABALHO_PLAYER_H
#define PROG2TRABALHO_PLAYER_H

#include <allegro5/allegro.h>
#include <stdbool.h>

typedef struct {
    float x, y;
    int largura_frame, altura_frame;
    int direcao, frame_atual;
    ALLEGRO_BITMAP *sprites[4];
    bool atacando;
    int timer_ataque;
} Personagem;

void inicializar_player_sprites(Personagem *p, float x, float y, const char *caminhos[4]);
void desenhar_personagem(Personagem p);
void destruir_personagem(Personagem *p);

#endif
