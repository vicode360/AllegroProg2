#include "nivel.h"
#include "mapa.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int nivel_atual = 1;

static const int kills_alvo[] = {0, 5, 10, 15, 15, 0};

static PokemonData pokemon_atual, pokemon_boss;
static PowerUp powerup_atual;
static bool pokemon_ok = false;
static char pokemon_hud[128];

typedef struct {
    int id;
    PokemonData pokemon;
    char sprite_path[64];
    ALLEGRO_BITMAP *sprite;
    ALLEGRO_THREAD *thread;
    ALLEGRO_MUTEX *mutex;
    bool done;
    bool ok;
    bool quer_sprite;
} CargaPokemon;

static CargaPokemon carga_pickup;
static PokemonPickup pickup;
static CargaPokemon carga_boss;
static CargaPokemon carga_pokedex[POKEDEX_TOTAL];
static int pokedex_pagina = 0;
static bool pokedex_iniciada = false;
static int temp_contador = 0;

typedef struct {
    ObjetivoTipo tipo;
    bool concluido;
    bool falhou;
    int kills_com_poder;
    bool coletou;
} Objetivo;

static Objetivo objetivo;
static int objetivos_concluidos = 0;
static bool preservar_pokemon = false;

static void *carga_worker(ALLEGRO_THREAD *t, void *arg) {
    (void)t;
    CargaPokemon *c = (CargaPokemon *)arg;
    bool ok = buscar_pokemon(c->id, &c->pokemon);
    if (ok) {
        buscar_descricao(c->id, c->pokemon.descricao, sizeof(c->pokemon.descricao));
        if (c->quer_sprite) baixar_sprite(c->id, c->sprite_path);
    }
    al_lock_mutex(c->mutex);
    c->ok = ok;
    c->done = true;
    al_unlock_mutex(c->mutex);
    return NULL;
}

static bool carga_pronta(const CargaPokemon *c) {
    al_lock_mutex(c->mutex);
    bool done = c->done;
    al_unlock_mutex(c->mutex);
    return done;
}

static void carga_iniciar(CargaPokemon *c, int id, bool sprite) {
    c->id = id;
    c->done = false;
    c->ok = false;
    c->sprite = NULL;
    c->quer_sprite = sprite;
    if (!c->mutex) c->mutex = al_create_mutex();
    if (sprite) snprintf(c->sprite_path, sizeof(c->sprite_path), "/tmp/poke_%d.png", temp_contador++);
    c->thread = al_create_thread(carga_worker, c);
    if (c->thread) {
        al_start_thread(c->thread);
    } else {
        bool ok = buscar_pokemon(c->id, &c->pokemon);
        if (ok) {
            buscar_descricao(c->id, c->pokemon.descricao, sizeof(c->pokemon.descricao));
            if (c->quer_sprite) baixar_sprite(c->id, c->sprite_path);
        }
        c->ok = ok;
        c->done = true;
    }
}

static void carga_finalizar(CargaPokemon *c) {
    if (c->thread && carga_pronta(c)) {
        al_join_thread(c->thread, NULL);
        c->thread = NULL;
        if (c->ok && c->quer_sprite && !c->sprite) {
            c->sprite = al_load_bitmap(c->sprite_path);
            remove(c->sprite_path);
        }
    }
}

static void carga_destruir(CargaPokemon *c) {
    if (c->thread) { al_join_thread(c->thread, NULL); c->thread = NULL; }
    if (c->sprite) { al_destroy_bitmap(c->sprite); c->sprite = NULL; }
    if (c->mutex) { al_destroy_mutex(c->mutex); c->mutex = NULL; }
}

static void resetar_powerup(void) {
    pokemon_ok = false;
    powerup_atual.habilidade = HAB_NENHUMA;
    powerup_atual.tipo[0] = '\0';
    pokemon_hud[0] = '\0';
    escudo_ativo = false;
}

static void pickup_destruir_sprite(void) {
    if (pickup.sprite) { al_destroy_bitmap(pickup.sprite); pickup.sprite = NULL; }
}

static void spawn_pickup(void) {
    pickup_destruir_sprite();
    carga_destruir(&carga_pickup);
    pickup.ativo = true;
    pickup.pronto = false;

    int id = (rand() % 151) + 1;
    int w = COLUNAS_MAPA * TAM_TILE;
    int h = LINHAS_MAPA * TAM_TILE;
    int canto = rand() % 4;
    const float margem = 70.f;
    switch (canto) {
        case 0:  pickup.x = margem;      pickup.y = margem;      break;
        case 1:  pickup.x = w - margem;  pickup.y = margem;      break;
        case 2:  pickup.x = margem;      pickup.y = h - margem;  break;
        default: pickup.x = w - margem;  pickup.y = h - margem;  break;
    }
    carga_iniciar(&carga_pickup, id, true);
}

void nivel_pickup_atualizar(void) {
    carga_finalizar(&carga_pickup);
    if (carga_pickup.done && !pickup.pronto) {
        pickup.pokemon = carga_pickup.pokemon;
        if (carga_pickup.ok) {
            pickup.powerup = calcular_powerup(&carga_pickup.pokemon);
            pickup.sprite = carga_pickup.sprite;
            carga_pickup.sprite = NULL;
        } else {
            pickup.powerup.habilidade = HAB_NENHUMA;
            pickup.powerup.tipo[0] = '\0';
            pickup.sprite = NULL;
            snprintf(pickup.pokemon.name, sizeof(pickup.pokemon.name), "???");
        }
        pickup.pronto = true;
    }
}

static void sortear_objetivo(void) {
    static const ObjetivoTipo pool[] = {OBJ_COMBO_3, OBJ_COLETAR, OBJ_KILLS_PODER};
    objetivo.tipo = pool[rand() % (int)(sizeof(pool) / sizeof(pool[0]))];
    objetivo.concluido = false;
    objetivo.falhou = false;
    objetivo.kills_com_poder = 0;
    objetivo.coletou = false;
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

int nivel_kills_alvo(void) { return kills_alvo[nivel_atual]; }

void nivel_iniciar(Inimigo inimigos[]) {
    srand((unsigned)time(NULL));
    spawn_inimigos_level(inimigos, nivel_atual);
    resetar_powerup();
    spawn_pickup();
    carga_iniciar(&carga_boss, 150, false);
    sortear_objetivo();
    objetivos_concluidos = 0;
    preservar_pokemon = false;
}

static void avaliar_objetivo_ao_completar(void) {
    if (objetivo.concluido || objetivo.falhou) return;
    switch (objetivo.tipo) {
        case OBJ_COLETAR:
            objetivo.concluido = objetivo.coletou;
            objetivo.falhou = !objetivo.coletou;
            break;
        case OBJ_KILLS_PODER:
            objetivo.concluido = objetivo.kills_com_poder >= 5;
            objetivo.falhou = !objetivo.concluido;
            break;
        default: break;
    }
}

void nivel_avancar(Inimigo inimigos[]) {
    avaliar_objetivo_ao_completar();
    if (objetivo.concluido) {
        objetivos_concluidos++;
        preservar_pokemon = true;
    } else {
        preservar_pokemon = false;
    }

    nivel_atual++;
    spawn_inimigos_level(inimigos, nivel_atual);
    if (!preservar_pokemon) resetar_powerup();
    spawn_pickup();
    sortear_objetivo();
}

void nivel_destruir(void) {
    pickup_destruir_sprite();
    carga_destruir(&carga_pickup);
    carga_destruir(&carga_boss);
    for (int i = 0; i < POKEDEX_TOTAL; i++) carga_destruir(&carga_pokedex[i]);
}

const PowerUp *nivel_powerup(void) { return &powerup_atual; }
const char *nivel_pokemon_hud(void) { return pokemon_hud; }
bool nivel_pokemon_ok(void) { return pokemon_ok; }
const PokemonData *nivel_boss(void) {
    carga_finalizar(&carga_boss);
    if (carga_boss.ok) pokemon_boss = carga_boss.pokemon;
    return &pokemon_boss;
}
bool nivel_boss_ok(void) {
    carga_finalizar(&carga_boss);
    return carga_boss.ok;
}

const PokemonPickup *nivel_pickup(void) { return &pickup; }

void nivel_tentar_coletar_pickup(float px, float py) {
    if (!pickup.ativo || !pickup.pronto) return;
    float dx = px - pickup.x, dy = py - pickup.y;
    if (dx * dx + dy * dy > 40.f * 40.f) return;
    if (!carga_pickup.ok) return;
    pickup.ativo = false;
    pokemon_atual = pickup.pokemon;
    powerup_atual = pickup.powerup;
    pokemon_ok = true;
    powerup_para_hud(&pokemon_atual, &powerup_atual, pokemon_hud, sizeof(pokemon_hud));
    objetivo.coletou = true;
}

void nivel_evento_slash_hit(int hits) {
    if (objetivo.concluido || objetivo.falhou) return;
    if (objetivo.tipo == OBJ_COMBO_3 && hits >= 3) {
        objetivo.concluido = true;
    }
}

void nivel_evento_kill(void) {
    if (pokemon_ok) objetivo.kills_com_poder++;
}

ObjetivoTipo nivel_objetivo_atual(void) { return objetivo.tipo; }

const char *nivel_objetivo_nome(void) {
    switch (objetivo.tipo) {
        case OBJ_COMBO_3:     return "Combo 3x (mate 3 com 1 golpe)";
        case OBJ_COLETAR:     return "Capture o pokemon";
        case OBJ_KILLS_PODER: return "Mate 5 com poder";
        default:              return "";
    }
}

bool nivel_objetivo_concluido(void) { return objetivo.concluido; }
bool nivel_objetivo_falhou(void) { return objetivo.falhou; }
int nivel_objetivos_concluidos(void) { return objetivos_concluidos; }

void nivel_carregar_pokedex(void) {
    if (pokedex_iniciada) return;
    pokedex_iniciada = true;
    pokedex_pagina = 0;
    for (int i = 0; i < POKEDEX_TOTAL; i++) {
        carga_iniciar(&carga_pokedex[i], (rand() % 151) + 1, true);
    }
}

void nivel_pokedex_atualizar(void) {
    for (int i = 0; i < POKEDEX_TOTAL; i++) carga_finalizar(&carga_pokedex[i]);
}

int nivel_pokedex_total(void) { return POKEDEX_TOTAL; }
int nivel_pokedex_pagina_atual(void) { return pokedex_pagina; }

void nivel_pokedex_proxima(void) {
    pokedex_pagina = (pokedex_pagina + 1) % POKEDEX_TOTAL;
}

void nivel_pokedex_anterior(void) {
    pokedex_pagina = (pokedex_pagina - 1 + POKEDEX_TOTAL) % POKEDEX_TOTAL;
}

bool nivel_pokedex_pronto(int pagina) {
    carga_finalizar(&carga_pokedex[pagina]);
    return carga_pokedex[pagina].ok;
}

const PokemonData *nivel_pokedex_dados(int pagina) {
    return &carga_pokedex[pagina].pokemon;
}

ALLEGRO_BITMAP *nivel_pokedex_sprite(int pagina) {
    return carga_pokedex[pagina].sprite;
}
