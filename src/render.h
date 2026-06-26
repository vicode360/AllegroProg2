#ifndef PROG2TRABALHO_RENDER_H
#define PROG2TRABALHO_RENDER_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <stdbool.h>
#include "player.h"
#include "inimigo.h"
#include "pokeapi.h"

void render_tela_fim(ALLEGRO_FONT *font, int w, int h, bool vitoria);
void render_hud(ALLEGRO_FONT *font, int w, bool nivel_pronto);
void render_pokedex(ALLEGRO_FONT *font, const PokemonData *p);
void render_casa(ALLEGRO_BITMAP *casa, bool parte_frente);
void render_slash(Personagem player, ALLEGRO_BITMAP *frames[6]);

#endif
