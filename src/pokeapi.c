#include "pokeapi.h"
#include <curl/curl.h>
#include <cjson/cJSON.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *data; size_t size; } ResponseBuffer;

static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t total = size * nmemb;
    ResponseBuffer *buf = userp;
    char *tmp = realloc(buf->data, buf->size + total + 1);
    if (!tmp) return 0;
    buf->data = tmp;
    memcpy(buf->data + buf->size, contents, total);
    buf->size += total;
    buf->data[buf->size] = '\0';
    return total;
}

static int extrair_stat(cJSON *stats, const char *nome) {
    if (!cJSON_IsArray(stats)) return 0;
    cJSON *obj;
    cJSON_ArrayForEach(obj, stats) {
        cJSON *info = cJSON_GetObjectItem(obj, "stat");
        cJSON *n = info ? cJSON_GetObjectItem(info, "name") : NULL;
        if (n && cJSON_IsString(n) && strcmp(n->valuestring, nome) == 0) {
            cJSON *base = cJSON_GetObjectItem(obj, "base_stat");
            if (base && cJSON_IsNumber(base)) return base->valueint;
        }
    }
    return 0;
}

bool buscar_pokemon(int id, PokemonData *out) {
    if (!out) return false;
    memset(out, 0, sizeof(*out));
    out->id = id;

    char url[128];
    snprintf(url, sizeof(url), "https://pokeapi.co/api/v2/pokemon/%d", id);

    CURL *curl = curl_easy_init();
    if (!curl) return false;

    ResponseBuffer resp = {malloc(1), 0};
    if (!resp.data) { curl_easy_cleanup(curl); return false; }
    resp.data[0] = '\0';

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resp);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    bool ok = curl_easy_perform(curl) == CURLE_OK;
    curl_easy_cleanup(curl);
    if (!ok) { free(resp.data); return false; }

    cJSON *json = cJSON_Parse(resp.data);
    free(resp.data);
    if (!json) return false;

    cJSON *name = cJSON_GetObjectItem(json, "name");
    if (name && cJSON_IsString(name)) {
        strncpy(out->name, name->valuestring, sizeof(out->name) - 1);
        if (out->name[0] >= 'a' && out->name[0] <= 'z') out->name[0] -= 32;
    }

    cJSON *types = cJSON_GetObjectItem(json, "types");
    cJSON *first = types && cJSON_IsArray(types) ? cJSON_GetArrayItem(types, 0) : NULL;
    cJSON *type_obj = first ? cJSON_GetObjectItem(first, "type") : NULL;
    cJSON *type_name = type_obj ? cJSON_GetObjectItem(type_obj, "name") : NULL;
    if (type_name && cJSON_IsString(type_name))
        strncpy(out->type, type_name->valuestring, sizeof(out->type) - 1);

    cJSON *stats = cJSON_GetObjectItem(json, "stats");
    out->hp = extrair_stat(stats, "hp");
    out->attack = extrair_stat(stats, "attack");
    out->defense = extrair_stat(stats, "defense");
    out->speed = extrair_stat(stats, "speed");
    out->special_attack = extrair_stat(stats, "special-attack");
    out->special_defense = extrair_stat(stats, "special-defense");

    cJSON_Delete(json);
    return true;
}

bool buscar_descricao(int id, char *out, int max_len) {
    if (!out || max_len <= 0) return false;
    out[0] = '\0';

    char url[160];
    snprintf(url, sizeof(url), "https://pokeapi.co/api/v2/pokemon-species/%d", id);

    CURL *curl = curl_easy_init();
    if (!curl) return false;

    ResponseBuffer resp = {malloc(1), 0};
    if (!resp.data) { curl_easy_cleanup(curl); return false; }
    resp.data[0] = '\0';

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resp);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    bool ok = curl_easy_perform(curl) == CURLE_OK;
    curl_easy_cleanup(curl);
    if (!ok) { free(resp.data); return false; }

    cJSON *json = cJSON_Parse(resp.data);
    free(resp.data);
    if (!json) return false;

    cJSON *entries = cJSON_GetObjectItem(json, "flavor_text_entries");
    bool achou = false;
    if (cJSON_IsArray(entries)) {
        cJSON *obj;
        cJSON_ArrayForEach(obj, entries) {
            cJSON *lang = cJSON_GetObjectItem(obj, "language");
            cJSON *lname = lang ? cJSON_GetObjectItem(lang, "name") : NULL;
            if (lname && cJSON_IsString(lname) && strcmp(lname->valuestring, "en") == 0) {
                cJSON *ft = cJSON_GetObjectItem(obj, "flavor_text");
                if (ft && cJSON_IsString(ft)) {
                    strncpy(out, ft->valuestring, max_len - 1);
                    out[max_len - 1] = '\0';
                    for (char *p = out; *p; p++) {
                        if (*p == '\n' || *p == '\f' || *p == '\r') *p = ' ';
                    }
                    achou = true;
                    break;
                }
            }
        }
    }
    cJSON_Delete(json);
    return achou;
}

PowerUp calcular_powerup(PokemonData *p) {
    PowerUp pu;
    pu.habilidade = HAB_NENHUMA;
    pu.tipo[0] = '\0';
    if (!p) return pu;
    strncpy(pu.tipo, p->type, sizeof(pu.tipo) - 1);

    static const struct { const char *tipo; Habilidade hab; } mapa[] = {
        {"fire",      HAB_AREA},
        {"fighting",  HAB_AREA},
        {"bug",       HAB_AREA},
        {"water",     HAB_CONGELAR},
        {"ice",       HAB_CONGELAR},
        {"poison",    HAB_CONGELAR},
        {"electric",  HAB_VELOCIDADE},
        {"grass",     HAB_REGEN_ESCUDO},
        {"fairy",     HAB_REGEN_ESCUDO},
        {"rock",      HAB_ALCANCE},
        {"ground",    HAB_ALCANCE},
        {"steel",     HAB_ALCANCE},
        {"psychic",   HAB_DANO_DOBRO},
        {"dragon",    HAB_DANO_DOBRO},
        {"ghost",     HAB_DANO_DOBRO},
        {"dark",      HAB_DANO_DOBRO},
        {"flying",    HAB_KNOCKBACK},
        {"normal",    HAB_KNOCKBACK},
    };
    for (int i = 0; i < (int)(sizeof(mapa) / sizeof(mapa[0])); i++) {
        if (strcmp(p->type, mapa[i].tipo) == 0) { pu.habilidade = mapa[i].hab; break; }
    }
    return pu;
}

const char *habilidade_nome(Habilidade h) {
    switch (h) {
        case HAB_AREA:        return "Area";
        case HAB_CONGELAR:    return "Congelar";
        case HAB_VELOCIDADE:  return "Velocidade+";
        case HAB_REGEN_ESCUDO:return "Regen Escudo";
        case HAB_ALCANCE:     return "Alcance+";
        case HAB_DANO_DOBRO:  return "Dano Dobro";
        case HAB_KNOCKBACK:   return "Knockback";
        default:              return "Nenhuma";
    }
}

void powerup_para_hud(PokemonData *p, PowerUp *pu, char *buf, int len) {
    if (!p || !pu || !buf) return;
    if (pu->habilidade == HAB_NENHUMA) {
        snprintf(buf, len, "[%s]", p->name);
    } else {
        snprintf(buf, len, "[%s] %s", p->name, habilidade_nome(pu->habilidade));
    }
}

static size_t write_file_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    FILE *f = userp;
    return fwrite(contents, size, nmemb, f);
}

bool baixar_sprite(int id, const char *arquivo) {
    if (id <= 0 || !arquivo) return false;

    char url[160];
    snprintf(url, sizeof(url),
             "https://raw.githubusercontent.com/PokeAPI/sprites/master/sprites/pokemon/%d.png", id);

    CURL *curl = curl_easy_init();
    if (!curl) return false;

    FILE *f = fopen(arquivo, "wb");
    if (!f) { curl_easy_cleanup(curl); return false; }

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_file_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, f);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    fclose(f);

    if (res != CURLE_OK) { remove(arquivo); return false; }
    return true;
}
