
#ifndef PROG2TRABALHO_COLISAO_H
#define PROG2TRABALHO_COLISAO_H
#include <stdbool.h>

bool checar_colisao(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2);
bool resolver_colisoes(int next_x, int next_y, int direcao, int mapas);

#endif  // PROG2TRABALHO_COLISAO_H
