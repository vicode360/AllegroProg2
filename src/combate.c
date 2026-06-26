#include "combate.h"
#include <math.h>

void combate_aplicar_slash(Personagem *player, const PowerUp *pu, Inimigo inimigos[], int num) {
    if (player->timer_ataque != 14) return;

    float pc_x = player->x + 55, pc_y = player->y + 50;
    float face_angle;
    switch (player->direcao) {
        case 0: face_angle = (float)(M_PI / 2.0);  break;
        case 1: face_angle = (float)(-M_PI / 2.0); break;
        case 2: face_angle = (float)M_PI;            break;
        default: face_angle = 0;                     break;
    }

    float radius = pu->slash_maior ? 120.f : 80.f;
    int dano = pu->dano_dobro ? 30 : 15;

    for (int i = 0; i < num; i++) {
        if (!inimigos[i].ativo || inimigos[i].estado == ESTADO_DIE) continue;

        float ec_x = inimigos[i].x + (inimigos[i].is_boss ? 90 : 45);
        float ec_y = inimigos[i].y + (inimigos[i].is_boss ? 64 : 32);
        float dx = ec_x - pc_x, dy = ec_y - pc_y;
        float dist = sqrtf(dx * dx + dy * dy);
        if (dist > radius) continue;

        float diff = atan2f(dy, dx) - face_angle;
        while (diff > (float)M_PI) diff -= (float)(2 * M_PI);
        while (diff < (float)-M_PI) diff += (float)(2 * M_PI);

        if (fabsf(diff) > (float)(M_PI / 2.0)) continue;

        inimigos[i].hp -= dano;
        if (inimigos[i].hp <= 0) {
            inimigos[i].estado = ESTADO_DIE;
            inimigos[i].frame_atual = 0.f;
        } else if (inimigos[i].estado != ESTADO_HURT) {
            inimigos[i].estado = ESTADO_HURT;
            inimigos[i].frame_atual = 0.f;
        }
    }
}
