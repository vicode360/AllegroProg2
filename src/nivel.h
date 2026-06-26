#ifndef PROG2TRABALHO_NIVEL_H
#define PROG2TRABALHO_NIVEL_H

#include <stdbool.h>
#include "inimigo.h"
#include "pokeapi.h"

extern int nivel_atual;

void nivel_iniciar(Inimigo inimigos[]);
void nivel_avancar(Inimigo inimigos[]);
bool nivel_completo(Inimigo inimigos[]);

const PowerUp *nivel_powerup(void);
const char *nivel_pokemon_hud(void);
bool nivel_pokemon_ok(void);
const PokemonData *nivel_boss(void);
bool nivel_boss_ok(void);

void nivel_carregar_pokedex(void);
const PokemonData *nivel_pokedex(void);
bool nivel_pokedex_ok(void);

#endif
