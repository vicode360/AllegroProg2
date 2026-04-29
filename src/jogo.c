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
            if (colisao == false) {
                player.x = next_x;
                player.y = next_y;
            }
            if (player.x > 460) {
                player.x = -50;
            }
            else if (player.x < -65) {
                player.x = 440;
            }
            if (player.y > altura_tela) player.y = -50;
            else if (player.y < -65) player.y = 440;
                if(redraw && al_is_event_queue_empty(queue))
                {
                    gato.x = player.x + 60;
                    gato.y = player.y + 85;

                    al_clear_to_color(al_map_rgb(0, 0, 0));
                    desenhar_mapa(tileset);
                    al_rest(0.01);
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
        al_destroy_bitmap(tileset);;
        al_destroy_font(font);
        al_destroy_display(disp);
        al_destroy_timer(timer);
        al_destroy_event_queue(queue);
}