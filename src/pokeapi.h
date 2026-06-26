#ifndef PROG2TRABALHO_POKEAPI_H
#define PROG2TRABALHO_POKEAPI_H

#include <stdbool.h>

typedef struct {
    int id;
    char name[64];
    char type[32];
    char descricao[256];
    int hp, attack, defense, speed, special_attack, special_defense;
} PokemonData;

typedef enum {
    HAB_NENHUMA,
    HAB_AREA,
    HAB_CONGELAR,
    HAB_VELOCIDADE,
    HAB_REGEN_ESCUDO,
    HAB_ALCANCE,
    HAB_DANO_DOBRO,
    HAB_KNOCKBACK
} Habilidade;

typedef struct {
    char tipo[32];
    Habilidade habilidade;
} PowerUp;

bool buscar_pokemon(int id, PokemonData *out);
bool buscar_descricao(int id, char *out, int max_len);
bool baixar_sprite(int id, const char *arquivo);
PowerUp calcular_powerup(PokemonData *pokemon);
const char *habilidade_nome(Habilidade h);
void powerup_para_hud(PokemonData *pokemon, PowerUp *pu, char *buffer, int max_len);

#endif
