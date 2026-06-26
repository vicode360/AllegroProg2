#include "nivel.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int nivel_atual = 1;

static const int kills_alvo[] = {0, 5, 10, 15, 15, 0};

static PokemonData pokemon_atual, pokemon_boss, pokemon_casa;
static PowerUp powerup_atual;
static bool pokemon_ok = false, boss_ok = false, pokedex_ok = false;
static char pokemon_hud[128];

static void buscar_powerup(void) {
    int id = (rand() % 151) + 1;
    pokemon_ok = buscar_pokemon(id, &pokemon_atual);
    if (pokemon_ok) {
        powerup_atual = calcular_powerup(&pokemon_atual);
        powerup_para_hud(&pokemon_atual, &powerup_atual, pokemon_hud, sizeof(pokemon_hud));
        escudo_ativo = powerup_atual.escudo;
    } else {
        powerup_atual = (PowerUp){false, false, false, false};
        snprintf(pokemon_hud, sizeof(pokemon_hud), "[Sem conexao]");
        escudo_ativo = false;
    }
}

static bool boss_morto(Inimigo inimigos[]) {
    if (nivel_atual != 4) return true;
    for (int i = 0; i < num_inimigos_atual; i++) {
        if (inimigos[i].is_boss) return !inimigos[i].ativo;
    }
    return true;
}

bool nivel_completo(Inimigo inimigos[]) {
    if (nivel_atual >= 5) return false;
    if (kills_no_level < kills_alvo[nivel_atual]) return false;
    return nivel_atual != 4 || boss_morto(inimigos);
}

void nivel_iniciar(Inimigo inimigos[]) {
    srand((unsigned)time(NULL));
    spawn_inimigos_level(inimigos, nivel_atual);
    buscar_powerup();
    boss_ok = buscar_pokemon(150, &pokemon_boss);
}

void nivel_avancar(Inimigo inimigos[]) {
    nivel_atual++;
    spawn_inimigos_level(inimigos, nivel_atual);
    buscar_powerup();
}

const PowerUp *nivel_powerup(void) { return &powerup_atual; }
const char *nivel_pokemon_hud(void) { return pokemon_hud; }
bool nivel_pokemon_ok(void) { return pokemon_ok; }
const PokemonData *nivel_boss(void) { return &pokemon_boss; }
bool nivel_boss_ok(void) { return boss_ok; }

void nivel_carregar_pokedex(void) {
    if (pokedex_ok) return;
    pokedex_ok = buscar_pokemon((rand() % 151) + 1, &pokemon_casa);
}

const PokemonData *nivel_pokedex(void) { return &pokemon_casa; }
bool nivel_pokedex_ok(void) { return pokedex_ok; }
