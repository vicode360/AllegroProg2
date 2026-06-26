#ifndef PROG2TRABALHO_NIVEL_H
#define PROG2TRABALHO_NIVEL_H

#include <stdbool.h>
#include <allegro5/allegro.h>
#include "inimigo.h"
#include "pokeapi.h"

extern int nivel_atual;

void nivel_iniciar(Inimigo inimigos[]);
void nivel_avancar(Inimigo inimigos[]);
void nivel_destruir(void);
bool nivel_completo(Inimigo inimigos[]);
int nivel_kills_alvo(void);

const PowerUp *nivel_powerup(void);
const char *nivel_pokemon_hud(void);
bool nivel_pokemon_ok(void);
const PokemonData *nivel_boss(void);
bool nivel_boss_ok(void);

typedef enum {
    OBJ_NENHUM,
    OBJ_COMBO_3,
    OBJ_COLETAR,
    OBJ_KILLS_PODER
} ObjetivoTipo;

void nivel_evento_slash_hit(int hits);
void nivel_evento_kill(void);
ObjetivoTipo nivel_objetivo_atual(void);
const char *nivel_objetivo_nome(void);
bool nivel_objetivo_concluido(void);
bool nivel_objetivo_falhou(void);
int nivel_objetivos_concluidos(void);

typedef struct {
    float x, y;
    bool ativo;
    bool pronto;
    PokemonData pokemon;
    PowerUp powerup;
    ALLEGRO_BITMAP *sprite;
} PokemonPickup;

const PokemonPickup *nivel_pickup(void);
void nivel_pickup_atualizar(void);
void nivel_tentar_coletar_pickup(float px, float py);

#define POKEDEX_TOTAL 5

void nivel_carregar_pokedex(void);
void nivel_pokedex_atualizar(void);
int nivel_pokedex_total(void);
int nivel_pokedex_pagina_atual(void);
void nivel_pokedex_proxima(void);
void nivel_pokedex_anterior(void);
bool nivel_pokedex_pronto(int pagina);
const PokemonData *nivel_pokedex_dados(int pagina);
ALLEGRO_BITMAP *nivel_pokedex_sprite(int pagina);

#endif
