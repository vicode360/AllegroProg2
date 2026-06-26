
#ifndef PROG2TRABALHO_INTERIOR_H
#define PROG2TRABALHO_INTERIOR_H

#include <stdbool.h>
#include <allegro5/allegro.h>

void inicializar_interior(void);
void destruir_interior(void);
void desenhar_interior(int altura_tela, int largura_tela);
bool colisao_moveis(int nx, int ny, int ox, int oy, int pw, int ph);
bool colisao_parede(int x, int y, int dir);

#endif
