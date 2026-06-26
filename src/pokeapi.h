#ifndef PROG2TRABALHO_POKEAPI_H
#define PROG2TRABALHO_POKEAPI_H

#include <stdbool.h>

typedef struct {
    char name[64];
    char type[32];
    int hp, attack, defense, speed, special_attack, special_defense;
} PokemonData;

typedef struct {
    bool dano_dobro, velocidade_extra, escudo, slash_maior;
} PowerUp;

bool buscar_pokemon(int id, PokemonData *out);
PowerUp calcular_powerup(PokemonData *pokemon);
void powerup_para_hud(PokemonData *pokemon, PowerUp *pu, char *buffer, int max_len);

#endif
