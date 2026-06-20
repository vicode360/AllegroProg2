#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <stdio.h>
#include "jogo.h"
#include "mapa.h"
#include "player.h"
#include "interior.h"
#include "colisao.h"
#include "inimigo.h"

int mapas = 1;

static void atualizar_movimento(Personagem *player, int *next_x, int *next_y, int em_movimento) {
    if (!em_movimento) return;

    if (player->direcao == 0) *next_y += 6;
    else if (player->direcao == 1) *next_y -= 6;
    else if (player->direcao == 2) *next_x -= 6;
    else if (player->direcao == 3) *next_x += 6;
}

static void verificar_bordas(Personagem *player, int largura_tela, int altura_tela) {
    if (mapas == 3) return;

    if (player->y > altura_tela - 60) player->y = altura_tela - 60;
    if (player->y < -20) player->y = -20;

    if (player->x > largura_tela - 52) {
        if (mapa_atual_node->prox != NULL) {
            mapa_atual_node = mapa_atual_node->prox;
            mapas = mapa_atual_node->id;
            carregar_mapa_node(mapa_atual_node);
            player->x = -40;
        } else {
            player->x = largura_tela - 52;
        }
    }
    else if (player->x < -50) {
        if (mapa_atual_node->ant != NULL) {
            mapa_atual_node = mapa_atual_node->ant;
            mapas = mapa_atual_node->id;
            carregar_mapa_node(mapa_atual_node);
            player->x = largura_tela - 60;
        } else {
            player->x = -50;
        }
    }
}

static void entrar_casa(Personagem *player) {
    if (mapas == 1 && player->x > 250 && player->x < 285 && player->y > 0 && player->y < 200) {
        FILE *arquivo = fopen("assets/mapas/mapa03.txt", "r");
        if(arquivo) {
            for(int i=0; i<8; i++)
                for(int j=0; j<8; j++)
                    fscanf(arquivo, "%d", &mapa[i][j]);
            fclose(arquivo);
        }
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

    // Instanciação dos Múltiplos Spritesheets do Golem
    ALLEGRO_BITMAP *golem_idle = al_load_bitmap("assets/img/Golem_1_idle.png");
    ALLEGRO_BITMAP *golem_walk = al_load_bitmap("assets/img/Golem_1_walk.png");
    ALLEGRO_BITMAP *golem_hurt = al_load_bitmap("assets/img/Golem_1_hurt.png");
    ALLEGRO_BITMAP *golem_die  = al_load_bitmap("assets/img/Golem_1_die.png");
    ALLEGRO_BITMAP *golem_attack = al_load_bitmap("assets/img/Golem_1_attack.png");

    if (!golem_idle || !golem_walk || !golem_hurt || !golem_die) {
        printf("ERRO FATAL: Nao foi possivel carregar as texturas Golem_1_*.png\n");
        exit(1);
    }

    inicializar_interior();
    inicializar_lista_mapas();

    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));

    Personagem player;
    const char* imagens_p[4] = {
        "assets/img/run_down.png",
        "assets/img/run_up.png",
        "assets/img/run_left.png",
        "assets/img/run_right.png"
    };
    inicializar_player_sprites(&player, 100, 100, imagens_p);
    player.largura_frame = 96;
    player.altura_frame = 80;

    // Inicialização Limpa do array de Inimigos
    Inimigo inimigos[NUM_INIMIGOS];
    inicializar_inimigos(inimigos, NUM_INIMIGOS);

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
                atualizar_movimento(&player, &next_x, &next_y, em_movimento);
            } else {
                player.frame_atual = 0;
            }

            // Engine de Colisão Central
            if (!resolver_colisoes(next_x, next_y, player.direcao, mapas)) {
                player.x = next_x;
                player.y = next_y;
            }

            verificar_bordas(&player, largura_tela, altura_tela);
            entrar_casa(&player);

            // Resolução Física de AI Golem
            if (mapas == 1) {
                atualizar_inimigos(inimigos, NUM_INIMIGOS, &player);
            }

            // Gatilho Condicional de Dano DpS (Damage per Second bypass)
            if (player.atacando) {
                player.timer_ataque--;
                if (player.timer_ataque == 14) {
                    int hit_x = player.x + 55;
                    int hit_y = player.y + 50;
                    if (player.direcao == 0) hit_y += 30;
                    else if (player.direcao == 1) hit_y -= 30;
                    else if (player.direcao == 2) hit_x -= 30;
                    else if (player.direcao == 3) hit_x += 30;

                    if (mapas == 1) {
                        for (int i = 0; i < NUM_INIMIGOS; i++) {
                            if (inimigos[i].ativo) {
                                // Hitbox adaptada (Hitbox da arma 40x40 vs Corpo do Golem 90x64)
                                if (checar_colisao(hit_x, hit_y, 40, 40, inimigos[i].x, inimigos[i].y, 90, 64)) {
                                    if (inimigos[i].estado != ESTADO_DIE) { // Nao ataca corpos mortos
                                        inimigos[i].hp -= 15;
                                        if (inimigos[i].hp <= 0) {
                                            inimigos[i].estado = ESTADO_DIE;
                                            inimigos[i].frame_atual = 0.0f;
                                        } else {
                                            if (inimigos[i].estado != ESTADO_HURT) {
                                                inimigos[i].estado = ESTADO_HURT;
                                                inimigos[i].frame_atual = 0.0f;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                if (player.timer_ataque <= 0) player.atacando = false;
            }

            // Main Pipeline
            if(redraw && al_is_event_queue_empty(queue)) {
                al_clear_to_color(al_map_rgb(0, 0, 0));
                desenhar_mapa(tileset);
                al_rest(0.01);

                if (mapas == 3) {
                    if (player.x > 170 && player.x < 210 && player.y > 300) {
                        mapa_atual_node = cabeca_lista;
                        mapas = mapa_atual_node->id;
                        carregar_mapa_node(mapa_atual_node);
                        player.x = 268;
                        player.y = 220;
                    }
                    desenhar_interior(altura_tela, largura_tela);
                }

                if (mapas == 1) {
                    al_draw_tinted_scaled_rotated_bitmap_region(casa, 0, 63, 80, 34, al_map_rgb(255, 255, 255), 1, 1, 250, 206, 2, 2, 0, 0);
                }

                if (mapas == 1) {
                    // Passagem dos múltiplos arquivos de texturas via ponteiros
                    desenhar_inimigos(inimigos, NUM_INIMIGOS, golem_idle, golem_walk, golem_hurt, golem_die, golem_attack);
                }

                desenhar_personagem(player);

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

    // Processo de Desalocação
    for(int i = 0; i < 6; i++) {
        if(slash[i]) al_destroy_bitmap(slash[i]);
    }

    al_destroy_bitmap(golem_idle);
    al_destroy_bitmap(golem_walk);
    al_destroy_bitmap(golem_hurt);
    al_destroy_bitmap(golem_die);
    al_destroy_bitmap(golem_attack);

    destruir_lista_mapas();
    destruir_interior();
    destruir_personagem(&player);
    al_destroy_bitmap(casa);
    al_destroy_bitmap(tileset);
    al_destroy_font(font);
    al_destroy_display(disp);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);
}