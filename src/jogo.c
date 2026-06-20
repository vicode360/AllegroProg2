#include <allegro5/allegro.h>
#include "jogo.h"
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include "mapa.h"
#include "player.h"
#include "interior.h"

int mapas = 1;

bool checar_colisao(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2) {
    if (x1 + w1 > x2 && x1 < x2 + w2 && y1 + h1 > y2 && y1 < y2 + h2) {
        return true;
    }
    return false;
}

static void trocar_mapa_externo(Personagem *player, int largura_tela, int altura_tela) {
    if (mapas == 1) {
        carregar_mapa("assets/mapas/mapa02.txt");
        mapas = 2;
    } else {
        carregar_mapa("assets/mapas/mapa01.txt");
        mapas = 1;
    }
    (void)largura_tela, (void)altura_tela;
}

static void atualizar_movimento(Personagem *player, int *next_x, int *next_y, int em_movimento) {
    if (!em_movimento) return;

    if (player->direcao == 0) *next_y += 6;
    else if (player->direcao == 1) *next_y -= 6;
    else if (player->direcao == 2) *next_x -= 6;
    else if (player->direcao == 3) *next_x += 6;
}

static bool resolver_colisoes(int next_x, int next_y, int direcao) {
    const int off_px = 55, off_py = 50, hit_pw = 10, hit_ph = 20;

    if (mapas == 1) {
        if (checar_colisao(next_x + off_px, next_y + off_py, hit_pw, hit_ph, 243, 130, 75*2, 50*2))
            return true;
    }
    if (mapas == 3) {
        if (colisao_parede(next_x, next_y, direcao))
            return true;
        if (colisao_moveis(next_x, next_y, off_px, off_py, hit_pw, hit_ph))
            return true;
    }
    return false;
}

static void verificar_bordas(Personagem *player, int largura_tela, int altura_tela) {
    if (mapas == 3) return;

    bool saiu = false;

    if      (player->x > largura_tela - 52) { player->x = -50; saiu = true; }
    else if (player->x < -65)               { player->x = largura_tela - 72; saiu = true; }

    if      (player->y > altura_tela)       { player->y = -50; saiu = true; }
    else if (player->y < -65)               { player->y = altura_tela - 72; saiu = true; }

    if (saiu) {
        if (mapas == 1) {
            carregar_mapa("assets/mapas/mapa02.txt");
            mapas = 2;
        } else {
            carregar_mapa("assets/mapas/mapa01.txt");
            mapas = 1;
        }
    }
}

static void entrar_casa(Personagem *player) {
    if (mapas == 1 && player->x > 250 && player->x < 285 && player->y > 0 && player->y < 200) {
        carregar_mapa("assets/mapas/mapa03.txt");
        mapas = 3;
        player->x = 200;
        player->y = 300;
    }
}

void iniciar_jogo() {
    al_init();
    al_init_primitives_addon();
    al_init_image_addon();
    al_install_keyboard();
    al_set_new_display_flags(ALLEGRO_OPENGL);

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 30.0);
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();

    int largura_tela = COLUNAS_MAPA * TAM_TILE;
    int altura_tela = LINHAS_MAPA * TAM_TILE;

    ALLEGRO_DISPLAY* disp = al_create_display(largura_tela, altura_tela);
    al_set_window_title(disp, ":D");

    ALLEGRO_FONT* font = al_create_builtin_font();
    ALLEGRO_BITMAP *casa = al_load_bitmap("assets/img/casa.png");
    ALLEGRO_BITMAP *tileset = al_load_bitmap("assets/img/TX Tileset Grass.png");

    ALLEGRO_BITMAP *slash[6];
    slash[0] = al_load_bitmap("assets/img/File1.png");
    slash[1] = al_load_bitmap("assets/img/File2.png");
    slash[2] = al_load_bitmap("assets/img/File3.png");
    slash[3] = al_load_bitmap("assets/img/File4.png");
    slash[4] = al_load_bitmap("assets/img/File5.png");
    slash[5] = al_load_bitmap("assets/img/File6.png");

    for (int i = 0; i < 6; i++) {
        if (!slash[i]) {
            printf("ERRO FATAL: Nao foi possivel carregar o frame do slash %d!\n", i);
            exit(1);
        }
    }

    inicializar_interior();

    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));

    Personagem gato;
    Personagem player;

    inicializar_player_sheet(&gato, 100, 100, "assets/img/nekos.png");
    gato.largura_frame = 160;
    gato.altura_frame = 152;

    const char* imagens_p[4] = {
        "assets/img/run_down.png",
        "assets/img/run_up.png",
        "assets/img/run_left.png",
        "assets/img/run_right.png"
    };
    inicializar_player_sprites(&player, 100, 100, imagens_p);
    player.largura_frame = 96;
    player.altura_frame = 80;

    carregar_mapa("assets/mapas/mapa01.txt");

    float frame = 0.f;
    int em_movimento = 0;
    bool redraw = true;
    ALLEGRO_EVENT event;

    al_start_timer(timer);

    while(1) {
        int next_x = player.x;
        int next_y = player.y;

        al_wait_for_event(queue, &event);

        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) break;

        if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
            switch (event.keyboard.keycode) {
                case ALLEGRO_KEY_M:
                    trocar_mapa_externo(&player, largura_tela, altura_tela);
                    player.x = largura_tela / 2;
                    player.y = altura_tela / 2;
                    break;

                case ALLEGRO_KEY_F:
                    if (!player.atacando) {
                        player.atacando = true;
                        player.timer_ataque = 15;
                    }
                    break;
            }
        }
        else if(event.type == ALLEGRO_EVENT_TIMER) {
            redraw = true;

            ALLEGRO_KEYBOARD_STATE estado_teclado;
            al_get_keyboard_state(&estado_teclado);

            em_movimento = 0;

            if (al_key_down(&estado_teclado, ALLEGRO_KEY_UP)) {
                player.direcao = 1;
                em_movimento = 1;
            } else if (al_key_down(&estado_teclado, ALLEGRO_KEY_DOWN)) {
                player.direcao = 0;
                em_movimento = 1;
            } else if (al_key_down(&estado_teclado, ALLEGRO_KEY_LEFT)) {
                player.direcao = 2;
                em_movimento = 1;
            } else if (al_key_down(&estado_teclado, ALLEGRO_KEY_RIGHT)) {
                player.direcao = 3;
                em_movimento = 1;
            }

            if (em_movimento) {
                frame += 0.4;
                if (frame > 4) frame -= 4;
                player.frame_atual = (int)frame;
                gato.frame_atual = (int)frame;

                atualizar_movimento(&player, &next_x, &next_y, em_movimento);
                gato.direcao = (int[]){0, 2, 3, 1}[player.direcao];
            } else {
                player.frame_atual = 0;
                gato.frame_atual = 0;
            }

            if (!resolver_colisoes(next_x, next_y, player.direcao)) {
                player.x = next_x;
                player.y = next_y;
            }
            verificar_bordas(&player, largura_tela, altura_tela);
            entrar_casa(&player);

            if (player.atacando) {
                player.timer_ataque--;
                if (player.timer_ataque == 14) {
                    int hit_x = player.x + 55;
                    int hit_y = player.y + 50;
                    int hit_w = 40;
                    int hit_h = 40;

                    if (player.direcao == 0) hit_y += 30;
                    else if (player.direcao == 1) hit_y -= 30;
                    else if (player.direcao == 2) hit_x -= 30;
                    else if (player.direcao == 3) hit_x += 30;

                    if (checar_colisao(hit_x, hit_y, hit_w, hit_h, gato.x + 60, gato.y + 65, 30, 30)) {
                        printf("acertou o alvo\n");
                    }
                }
                if (player.timer_ataque <= 0) {
                    player.atacando = false;
                }
            }

            if(redraw && al_is_event_queue_empty(queue)) {
                gato.x = player.x + 60;
                gato.y = player.y + 65;

                al_clear_to_color(al_map_rgb(0, 0, 0));
                desenhar_mapa(tileset);
                al_rest(0.01);

                if (mapas == 3) {
                    if (player.x > 170 && player.x < 210 && player.y > 300) {
                        carregar_mapa("assets/mapas/mapa01.txt");
                        mapas = 1;
                        player.x = 268;
                        player.y = 220;
                    }
                    desenhar_interior(altura_tela, largura_tela);
                }

                if (mapas == 1) {
                    al_draw_tinted_scaled_rotated_bitmap_region(casa, 0, 63, 80, 34, al_map_rgb(255, 255, 255), 1, 1, 250, 206, 2, 2, 0, 0);
                }

                desenhar_personagem(player);
                desenhar_personagem(gato);

                if (player.atacando) {
                    int frame_slash = 5 - (player.timer_ataque * 6 / 16);
                    if (frame_slash < 0) frame_slash = 0;
                    if (frame_slash > 5) frame_slash = 5;

                    float angulo = 0;
                    int flag = 0;
                    float offset_x = 0;
                    float offset_y = 0;

                    switch (player.direcao) {
                        case 0:
                            angulo = ALLEGRO_PI / 2.0;
                            flag = 0;
                            offset_y = 40;
                            break;
                        case 1:
                            angulo = -ALLEGRO_PI / 2.0;
                            flag = 0;
                            offset_y = -40;
                            break;
                        case 2:
                            angulo = 0;
                            flag = ALLEGRO_FLIP_HORIZONTAL;
                            offset_x = -40;
                            break;
                        case 3:
                            angulo = 0;
                            flag = 0;
                            offset_x = 40;
                            break;
                    }

                    int slash_w = al_get_bitmap_width(slash[frame_slash]);
                    int slash_h = al_get_bitmap_height(slash[frame_slash]);

                    float draw_x = player.x + 55 + offset_x;
                    float draw_y = player.y + 50 + offset_y;

                    al_draw_tinted_scaled_rotated_bitmap_region(
                        slash[frame_slash],
                        0, 0, slash_w, slash_h,
                        al_map_rgb(255, 255, 255),
                        slash_w / 2.0, slash_h / 2.0,
                        draw_x, draw_y,
                        0.2, 0.2,
                        angulo,
                        flag
                    );
                }

                if (mapas == 1) {
                    al_draw_tinted_scaled_rotated_bitmap_region(casa, 0, 0, 80, 63, al_map_rgb(255, 255, 255), 1, 1, 250, 80, 2, 2, 0, 0);
                }

                al_flip_display();
                redraw = false;
            }
        }
    }

    for(int i = 0; i < 6; i++) {
        if(slash[i]) al_destroy_bitmap(slash[i]);
    }

    destruir_interior();
    destruir_personagem(&gato);
    destruir_personagem(&player);

    al_destroy_bitmap(casa);
    al_destroy_bitmap(tileset);
    al_destroy_font(font);
    al_destroy_display(disp);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);
}