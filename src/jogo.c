// apenas trazendo o game loop e a inicializacao dos personagens pra ca
#include <allegro5/allegro.h>
#include "jogo.h"
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include "mapa.h"
#include "player.h"

int mapas = 1;
bool checar_colisao(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2) {
    if (x1 + w1 > x2 && x1 < x2 + w2 && y1 + h1 > y2 && y1 < y2 + h2) {
        return true;
    }
    return false;
}

void iniciar_jogo() {
    al_init();
    al_init_image_addon();
    al_install_keyboard();
    al_set_new_display_flags(ALLEGRO_OPENGL);


    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 30.0);
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();

    int largura_tela = COLUNAS_MAPA * TAM_TILE;
    int altura_tela = LINHAS_MAPA * TAM_TILE;

    ALLEGRO_DISPLAY* disp = al_create_display(largura_tela, altura_tela);
    al_set_window_title(disp, (":D"));
    ALLEGRO_FONT* font = al_create_builtin_font();
    ALLEGRO_BITMAP *casa = al_load_bitmap("assets/img/casa.png");
    ALLEGRO_BITMAP *tileset = al_load_bitmap("assets/img/TX Tileset Grass.png");
    ALLEGRO_BITMAP *chao_interior = al_load_bitmap("assets/img/InteriorChao.png");
    ALLEGRO_BITMAP *parede_cima = al_load_bitmap("assets/img/InteriorCima56x5.png");
    ALLEGRO_BITMAP *parede_lateral = al_load_bitmap("assets/img/InteriorParedeLateral8x57.png");
    ALLEGRO_BITMAP *moveis = al_load_bitmap("assets/img/interior.png");

    ALLEGRO_BITMAP *cama = al_create_sub_bitmap(moveis, 0, 35, 80, 90); //x, y, largura, altura
    ALLEGRO_BITMAP *mesa = al_create_sub_bitmap(moveis, 115, 405, 115, 80);
    ALLEGRO_BITMAP *tapete = al_create_sub_bitmap(moveis, 0, 660, 130, 90);
    ALLEGRO_BITMAP *vaso = al_create_sub_bitmap(moveis, 530, 530, 50, 70);
    ALLEGRO_BITMAP *bau = al_create_sub_bitmap(moveis, 610, 530, 60, 70);
    ALLEGRO_BITMAP *livro = al_create_sub_bitmap(moveis, 500, 15, 80, 110);


    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));

    float frame = 0.f;

    // inicializando os personagens

    Personagem gato;
    Personagem player;

    inicializar_player_sheet(&gato, 100, 100, "assets/img/nekos.png");
    gato.largura_frame =  160;
    gato.altura_frame =  152;

    const char* imagens_p[4] = {"assets/img/run_down.png", "assets/img/run_up.png", "assets/img/run_left.png", "assets/img/run_right.png"};

    inicializar_player_sprites(&player, 100, 100, imagens_p);
    player.largura_frame = 96;
    player.altura_frame = 80;
    int em_movimento = 0;
    carregar_mapa("assets/mapas/mapa01.txt");

    bool redraw = true;
    ALLEGRO_EVENT event;

    al_start_timer(timer);
    while(1) {
        int next_x = player.x;
        int next_y = player.y;
        al_wait_for_event(queue, &event);
        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            break;
        }
        if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
            em_movimento = 1;
            switch (event.keyboard.keycode) {
                case ALLEGRO_KEY_UP: player.direcao = 1; break;
                case ALLEGRO_KEY_DOWN: player.direcao = 0; break;
                case ALLEGRO_KEY_LEFT: player.direcao = 2; break;
                case ALLEGRO_KEY_RIGHT: player.direcao = 3; break;
                case ALLEGRO_KEY_M:
                    if (mapas == 1) {
                        mapas = 2;
                        carregar_mapa("assets/mapas/mapa02.txt");
                        player.x = largura_tela/2;
                        player.y = altura_tela/2;
                    }else {
                        mapas = 1;
                        carregar_mapa("assets/mapas/mapa01.txt");
                        player.x = largura_tela/2;
                        player.y = altura_tela/2;
                    }
                default:
                em_movimento = 0;
            }
        }
        else if (event.type == ALLEGRO_EVENT_KEY_UP) {
            em_movimento = 0;
        }
        else if(event.type == ALLEGRO_EVENT_TIMER) {
            redraw = true;


            if (em_movimento == 1) {


                frame += 0.4;
                if (frame > 4) frame -= 4;
                player.frame_atual = (int)frame;
                gato.frame_atual = (int)frame;


                if (player.direcao == 0) {
                    next_y += 6;
                    gato.direcao = 0;
                } else if (player.direcao == 1) {
                    next_y -= 6;
                    gato.direcao = 2;
                } else if (player.direcao == 2) {
                    next_x -= 6;
                    gato.direcao = 3;
                } else if (player.direcao == 3) {
                    next_x += 6;
                    gato.direcao = 1;
                }
            }
            else {
                player.frame_atual = 0;
                gato.frame_atual = 0;
            }
            bool colisao = false;

            // hitbox do player
            int off_px = 35;  // deslocamento horizontal ate o hitbox
            int off_py = 50;  // deslocamento vertical ate o hitbox
            int hit_pw = 26;  // largura do hitbox
            int hit_ph = 20;  // altura do hitbox
            // hitbox d a casa
            int larg_casa = 75*2;
            int alt_casa = 50*2;
            int casa_x = 243;
            int casa_y = 130;
            if (mapas == 1 && checar_colisao(next_x + off_px, next_y+off_py, hit_pw, hit_ph, casa_x, casa_y, larg_casa, alt_casa)) {
                colisao = true;
            }

            if (mapas == 3) {
                if ((player.x < 85 && player.direcao == 2)  ||
                    (player.x > 300 && player.direcao == 3) ||
                    (player.y < 100 && player.direcao == 1) ||
                    ((player.y > 295 && player.direcao == 0) && ((player.x < 170) || (player.x > 210)) )) {
                    colisao = true;
                }else {
                    colisao = false;
                }
            }

            if (colisao == false) {
                player.x = next_x;
                player.y = next_y;
            }
            if (player.x > 460) {
                player.x = -50;
                if (mapas==1) {
                    carregar_mapa("assets/mapas/mapa02.txt");
                    mapas = 2;
                }else {
                    carregar_mapa("assets/mapas/mapa01.txt");
                    mapas = 1;
                }
            }
            else if (player.x < -65) {
                player.x = 440;
                if (mapas==1) {
                    carregar_mapa("assets/mapas/mapa02.txt");
                    mapas = 2;
                }else {
                    carregar_mapa("assets/mapas/mapa01.txt");
                    mapas = 1;
                }
            }
            if (player.y > altura_tela) {
                player.y = -50;
                if (mapas==1) {
                    carregar_mapa("assets/mapas/mapa02.txt");
                    mapas = 2;
                }else {
                    carregar_mapa("assets/mapas/mapa01.txt");
                    mapas = 1;
                }
            }
            else if (player.y < -65) {
                player.y = 440;
                if (mapas==1) {
                    carregar_mapa("assets/mapas/mapa02.txt");
                    mapas = 2;
                }else {
                    carregar_mapa("assets/mapas/mapa01.txt");
                    mapas = 1;
                }
            }if (mapas == 1 && player.x > 250 && player.x < 285 && player.y < 200 && player.y > 0) {
                carregar_mapa("assets/mapas/mapa03.txt");
                mapas = 3;

                player.x = 200;
                player.y = 300;
//                next_x = player.x;
//                next_y = player.y;
            }
            if (mapas == 3 && player.x > 250 && player.x < 285 && player.y < 200 && player.y > 0) {

            }
                if(redraw && al_is_event_queue_empty(queue))
                {
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
                        //isso poderia ter sido feito em outra aba para melhorar a organização? com certeza.
                        //chao da casa
                        int chao_w = al_get_bitmap_width(chao_interior);
                        int chao_h = al_get_bitmap_height(chao_interior);
                        int tamanho_do_chao_x = 8*chao_w;
                        int tamanho_do_chao_y = 8*chao_h;
                        int inicio_x = (largura_tela - tamanho_do_chao_x)/2;
                        int inicio_y = (altura_tela - tamanho_do_chao_y)/2;

                        for (int linha = 0; linha < 8; linha++) {
                            for (int coluna = 0; coluna < 8; coluna++) {
                                int pos_x = inicio_x + coluna*chao_w;
                                int pos_y = inicio_y + linha*chao_h;
                                al_draw_bitmap(chao_interior, pos_x, pos_y, 0);
                            }
                        }
                        //paredes do interior da casa
                        int p_cima_w = al_get_bitmap_width(parede_cima);
                        int p_cima_h = al_get_bitmap_height(parede_cima);
                        int p_lat_w = al_get_bitmap_width(parede_lateral);
                        int p_lat_h = al_get_bitmap_height(parede_lateral);
                        int p_bx_w = al_get_bitmap_width(parede_cima);
                        int p_bx_h = al_get_bitmap_height(parede_cima);

                        al_draw_scaled_bitmap(parede_cima, 0, 0, p_cima_w, p_cima_h, inicio_x - p_lat_w, inicio_y - p_cima_h, tamanho_do_chao_x + (p_lat_w * 2), p_cima_h, 0);
                        al_draw_scaled_bitmap(parede_lateral, 0, 0, p_lat_w, p_lat_h, inicio_x - p_lat_w, inicio_y, p_lat_w, tamanho_do_chao_y, ALLEGRO_FLIP_HORIZONTAL);
                        al_draw_scaled_bitmap(parede_lateral, 0, 0, p_lat_w, p_lat_h, inicio_x + tamanho_do_chao_x, inicio_y, p_lat_w, tamanho_do_chao_y, 0);
                        int largura_esq = (tamanho_do_chao_x / 2) - chao_w + p_lat_w;
                        al_draw_scaled_bitmap(parede_cima, 0, 0, p_bx_w, p_bx_h, inicio_x - p_lat_w, inicio_y + tamanho_do_chao_y, largura_esq, p_bx_h, 0);
                        int inicio_direita = inicio_x + (tamanho_do_chao_x / 2) + chao_w;
                        int largura_dir = (tamanho_do_chao_x / 2) - chao_w + p_lat_w;
                        al_draw_scaled_bitmap(parede_cima, 0, 0, p_bx_w, p_bx_h, inicio_direita, inicio_y + tamanho_do_chao_y, largura_dir, p_bx_h, 0);

                        if (player.y > 200) {
                            next_y = player.y;
                        }

                        al_draw_bitmap(cama, 115, 110, 0);
                        al_draw_bitmap(mesa, 250, 200, 0);
                        al_draw_bitmap(tapete, 185, 290, 0);
                        al_draw_bitmap(vaso, 120, 300, 0);
                        al_draw_bitmap(bau, 225, 120, 0);
                        al_draw_bitmap(livro, 310, 70, 0);
                    }
                    if (mapas == 1) {
                        al_draw_tinted_scaled_rotated_bitmap_region(casa, 0, 63, 80, 34, al_map_rgb(255, 255, 255), 1, 1, 250, 206, 2, 2, 0, 0);
                    }
                    desenhar_personagem(player);
                    desenhar_personagem(gato);
                    if (mapas == 1) {
                        al_draw_tinted_scaled_rotated_bitmap_region(casa, 0, 0, 80, 63, al_map_rgb(255, 255, 255), 1, 1, 250, 80, 2, 2, 0, 0);

                    }
                    al_flip_display();
                    redraw = false;
                }
        }
    }

    al_destroy_bitmap(cama);
    al_destroy_bitmap(mesa);
    al_destroy_bitmap(tapete);
    al_destroy_bitmap(vaso);
    al_destroy_bitmap(bau);
    al_destroy_bitmap(livro);

    al_destroy_bitmap(casa);
    al_destroy_bitmap(tileset);
    al_destroy_bitmap(chao_interior);
    al_destroy_bitmap(parede_cima);
    al_destroy_bitmap(parede_lateral);
    al_destroy_bitmap(moveis);

    al_destroy_font(font);
    al_destroy_display(disp);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);
}