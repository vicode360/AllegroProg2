#ifndef PROG2TRABALHO_PLAYER_H
#define PROG2TRABALHO_PLAYER_H

#include <allegro5/allegro.h>

typedef struct {
    float x, y;
    float frame;
    int direcao;
    int current_frame_y;
    float scale;
    ALLEGRO_BITMAP* sprites[4]; // ---> caso venha varios png's
    ALLEGRO_BITMAP* sprite; // -----> caso venha um so
} Personagem;






#endif //PROG2TRABALHO_PLAYER_H
