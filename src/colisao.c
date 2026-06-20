#include "colisao.h"

#include <stdio.h>
#include <stdlib.h>
#include "colisao.h"
#include "interior.h"

bool checar_colisao(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2) {
    return (x1 + w1 > x2 && x1 < x2 + w2 && y1 + h1 > y2 && y1 < y2 + h2);
}

bool resolver_colisoes(int next_x, int next_y, int direcao, int mapas) {
    const int off_px = 55, off_py = 50, hit_pw = 10, hit_ph = 20;

    if (mapas == 1) {
        if (checar_colisao(next_x + off_px, next_y + off_py, hit_pw, hit_ph, 243, 130, 75*2, 50*2))
            return true;
    }
    if (mapas == 3) {
        if (colisao_parede(next_x, next_y, direcao))
            return true;
        if (colisao_moveis(next_x, next_y, off_px, off_py, hit_pw, hit_ph))
            return true;
    }
    return false;
}