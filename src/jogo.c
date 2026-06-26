#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include "jogo.h"
#include "mapa.h"
#include "player.h"
#include "interior.h"
#include "colisao.h"
#include "inimigo.h"
#include "nivel.h"
#include "combate.h"
#include "render.h"

static void atualizar_movimento(Personagem *p, int *nx, int *ny, int movendo) {
    if (!movendo) return;
    const PowerUp *pu = nivel_powerup();
    int vel = (pu && pu->habilidade == HAB_VELOCIDADE) ? 9 : 6;
    if (p->direcao == 0) *ny += vel;
    else if (p->direcao == 1) *ny -= vel;
    else if (p->direcao == 2) *nx -= vel;
    else *nx += vel;
}

void iniciar_jogo(void) {
    curl_global_init(CURL_GLOBAL_DEFAULT);
    al_init();
    al_init_primitives_addon();
    al_init_image_addon();
    al_install_keyboard();
    al_set_new_display_flags(ALLEGRO_OPENGL);

    ALLEGRO_TIMER *timer = al_create_timer(1.0 / 30.0);
    ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();
    int w = COLUNAS_MAPA * TAM_TILE, h = LINHAS_MAPA * TAM_TILE;
    ALLEGRO_DISPLAY *disp = al_create_display(w, h);
    al_set_window_title(disp, ":D");

    ALLEGRO_FONT *font = al_create_builtin_font();
    ALLEGRO_BITMAP *casa = al_load_bitmap("assets/img/casa.png");
    ALLEGRO_BITMAP *tileset = al_load_bitmap("assets/img/TX Tileset Grass.png");
    ALLEGRO_BITMAP *slash[6];
    for (int i = 0; i < 6; i++) {
        char path[64];
        snprintf(path, sizeof(path), "assets/img/File%d.png", i + 1);
        slash[i] = al_load_bitmap(path);
    }

    ALLEGRO_BITMAP *golem[] = {
        al_load_bitmap("assets/img/Golem_1_idle.png"),
        al_load_bitmap("assets/img/Golem_1_walk.png"),
        al_load_bitmap("assets/img/Golem_1_hurt.png"),
        al_load_bitmap("assets/img/Golem_1_die.png"),
        al_load_bitmap("assets/img/Golem_1_attack.png")
    };
    if (!golem[4]) golem[4] = golem[1];
    bool golem_ok = true;
    for (int i = 0; i < 4; i++) if (!golem[i]) golem_ok = false;
    if (!golem_ok) {
        printf("erro texturas Golem nao carregadas\n");
        exit(1);
    }

    inicializar_interior();
    inicializar_lista_mapas();
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));

    Personagem player;
    const char *sprites_p[4] = {
        "assets/img/run_down.png", "assets/img/run_up.png",
        "assets/img/run_left.png", "assets/img/run_right.png"
    };
    inicializar_player_sprites(&player, 100, 100, sprites_p);
    player.largura_frame = 96;
    player.altura_frame = 80;

    Inimigo inimigos[MAX_INIMIGOS];
    nivel_iniciar(inimigos);

    float frame = 0.f;
    int movendo = 0;
    bool redraw = true, venceu = false;
    ALLEGRO_EVENT ev;
    al_start_timer(timer);

    while (1) {
        int nx = player.x, ny = player.y;
        al_wait_for_event(queue, &ev);

        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) break;

        if (venceu || game_over) {
            if (ev.type == ALLEGRO_EVENT_KEY_DOWN) break;
            if (ev.type == ALLEGRO_EVENT_TIMER) render_tela_fim(font, w, h, venceu, nivel_objetivos_concluidos());
            continue;
        }

        if (ev.type == ALLEGRO_EVENT_KEY_DOWN && ev.keyboard.keycode == ALLEGRO_KEY_F) {
            if (!player.atacando) {
                player.atacando = true;
                player.timer_ataque = 15;
                player.slash_aplicado = false;
            }
        } else if (ev.type == ALLEGRO_EVENT_KEY_DOWN && ev.keyboard.keycode == ALLEGRO_KEY_Q) {
            if (mapa_id == 3) nivel_pokedex_anterior();
        } else if (ev.type == ALLEGRO_EVENT_KEY_DOWN && ev.keyboard.keycode == ALLEGRO_KEY_E) {
            if (mapa_id == 3) nivel_pokedex_proxima();
        } else if (ev.type == ALLEGRO_EVENT_TIMER) {
            redraw = true;
            ALLEGRO_KEYBOARD_STATE keys;
            al_get_keyboard_state(&keys);
            movendo = 0;

            if (al_key_down(&keys, ALLEGRO_KEY_UP))    { player.direcao = 1; movendo = 1; }
            else if (al_key_down(&keys, ALLEGRO_KEY_DOWN)) { player.direcao = 0; movendo = 1; }
            else if (al_key_down(&keys, ALLEGRO_KEY_LEFT)) { player.direcao = 2; movendo = 1; }
            else if (al_key_down(&keys, ALLEGRO_KEY_RIGHT)){ player.direcao = 3; movendo = 1; }

            if (movendo) {
                frame += 0.4f;
                if (frame > 4) frame -= 4;
                player.frame_atual = (int)frame;
                atualizar_movimento(&player, &nx, &ny, movendo);
            } else {
                player.frame_atual = 0;
            }

            if (!resolver_colisoes(nx, ny, player.direcao, mapa_id)) {
                player.x = nx;
                player.y = ny;
            }
            mapa_atualizar_bordas(&player, w, h, inimigos);
            mapa_tentar_entrar_casa(&player);
            mapa_tentar_sair_casa(&player);

            nivel_pickup_atualizar();
            if (mapa_id == 3) {
                nivel_carregar_pokedex();
                nivel_pokedex_atualizar();
            } else {
                atualizar_inimigos(inimigos, num_inimigos_atual, &player);
                nivel_tentar_coletar_pickup(player.x + 55, player.y + 50);
            }
            if (nivel_atual == 5) { venceu = true; continue; }

            if (player.atacando) {
                if (!player.slash_aplicado) {
                    int hits = combate_aplicar_slash(&player, nivel_powerup(), inimigos, num_inimigos_atual);
                    nivel_evento_slash_hit(hits);
                    player.slash_aplicado = true;
                }
                player.timer_ataque--;
                if (player.timer_ataque <= 0) player.atacando = false;
            }

            if (redraw && al_is_event_queue_empty(queue)) {
                al_clear_to_color(al_map_rgb(0, 0, 0));
                desenhar_mapa(tileset);

                if (mapa_id == 3) {
                    desenhar_interior(h, w);
                    int pag = nivel_pokedex_pagina_atual();
                    render_pokedex(font, pag, nivel_pokedex_total(),
                                   nivel_pokedex_dados(pag), nivel_pokedex_sprite(pag),
                                   nivel_pokedex_pronto(pag));
                }

                if (mapa_id == 1) render_casa(casa, false);
                if (mapa_id != 3) {
                    desenhar_inimigos(inimigos, num_inimigos_atual,
                        golem[0], golem[1], golem[2], golem[3], golem[4]);
                    render_pickup(font, nivel_pickup());
                }

                desenhar_personagem(player);
                render_slash(player, slash);
                if (mapa_id == 1) render_casa(casa, true);
                render_hud(font, w, nivel_completo(inimigos));
                al_flip_display();
                redraw = false;
            }
        }
    }

    for (int i = 0; i < 6; i++) if (slash[i]) al_destroy_bitmap(slash[i]);
    for (int i = 0; i < 4; i++) if (golem[i]) al_destroy_bitmap(golem[i]);
    destruir_lista_mapas();
    destruir_interior();
    nivel_destruir();
    destruir_personagem(&player);
    al_destroy_bitmap(casa);
    al_destroy_bitmap(tileset);
    al_destroy_font(font);
    al_destroy_display(disp);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);
    curl_global_cleanup();
}
