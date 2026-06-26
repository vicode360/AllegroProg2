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

PowerUp calcular_powerup(PokemonData *p) {
    PowerUp pu = {false, false, false, false};
    if (!p) return pu;
    if (p->attack >= 100) pu.dano_dobro = true;
    if (p->speed >= 90) pu.velocidade_extra = true;
    if (p->hp >= 80) pu.escudo = true;
    if (p->defense >= 80) pu.slash_maior = true;
    return pu;
}

void powerup_para_hud(PokemonData *p, PowerUp *pu, char *buf, int len) {
    if (!p || !pu || !buf) return;
    char buffs[64] = "";
    if (pu->dano_dobro) strcat(buffs, "ATK+ ");
    if (pu->velocidade_extra) strcat(buffs, "SPD+ ");
    if (pu->escudo) strcat(buffs, "SHIELD ");
    if (pu->slash_maior) strcat(buffs, "RANGE+ ");
    if (buffs[0]) {
        buffs[strlen(buffs) - 1] = '\0';
        snprintf(buf, len, "[%s] %s", p->name, buffs);
    } else {
        snprintf(buf, len, "[%s]", p->name);
    }
}
