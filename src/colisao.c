#include "colisao.h"
#include "interior.h"

bool checar_colisao(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2) {
    return x1 + w1 > x2 && x1 < x2 + w2 && y1 + h1 > y2 && y1 < y2 + h2;
}

bool resolver_colisoes(int nx, int ny, int dir, int mapa_id) {
    const int ox = 55, oy = 50, pw = 10, ph = 20;

    if (mapa_id == 1 &&
        checar_colisao(nx + ox, ny + oy, pw, ph, 243, 130, 150, 100))
        return true;

    if (mapa_id == 3) {
        if (colisao_parede(nx, ny, dir)) return true;
        if (colisao_moveis(nx, ny, ox, oy, pw, ph)) return true;
    }
    return false;
}
