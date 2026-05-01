
#ifndef PROG2TRABALHO_INTERIOR_H
#define PROG2TRABALHO_INTERIOR_H
#include <stdbool.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro.h>

void inicializar_interior();
void destruir_interior();
void desenhar_interior(int altura_tela, int largura_tela);
bool colisao_moveis(int next_x, int next_y, int off_px, int off_py, int hit_pw, int hit_ph);
bool colisao_parede(int x, int y, int direcao);

void debug_desenhar_hitboxes(int player_x, int player_y, int off_px, int off_py, int hit_pw, int hit_ph);

#endif  // PROG2TRABALHO_INTERIOR_H
