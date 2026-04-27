// apenas trazendo o game loop e a inicializacao dos personagens pra ca
#include <allegro5/allegro.h>
#include "jogo.h"
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include "mapa.h"
#include "player.h"

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

    inicializar_player_sprites(&player, 200, 200, imagens_p);
    player.largura_frame = 96;
    player.altura_frame = 80;



    bool redraw = true;
    ALLEGRO_EVENT event;

    al_start_timer(timer);
    while(1)
    {
        al_wait_for_event(queue, &event);

        if(event.type == ALLEGRO_EVENT_TIMER) {
            redraw = true;
            frame += 0.2;
            if (frame > 4) frame -= 4;
            player.frame_atual = (int)frame;
            gato.frame_atual = (int)frame;
        }

        else if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
            break;
        else if(event.keyboard.keycode == ALLEGRO_KEY_RIGHT) {
            player.direcao = 3;
            gato.direcao = 1;
            player.x += 6;
        }
        else if(event.keyboard.keycode == ALLEGRO_KEY_LEFT) {
            player.direcao = 2;
            gato.direcao = 3;
            player.x -= 6;
        }
        else if(event.keyboard.keycode == ALLEGRO_KEY_UP) {
            player.direcao = 1;
            gato.direcao = 2;
            player.y -= 6;
        }
        else if(event.keyboard.keycode == ALLEGRO_KEY_DOWN) {
            player.direcao = 0;
            gato.direcao = 0;
            player.y += 6;
        }

        if(redraw && al_is_event_queue_empty(queue))
        {
            gato.x = player.x + 60;
            gato.y = player.y + 85;

            al_clear_to_color(al_map_rgb(0, 0, 0));
            carregar_mapa();
            desenhar_mapa(tileset);
            al_rest(0.01);
            al_draw_tinted_scaled_rotated_bitmap_region(casa, 0, 0, 80, 97, al_map_rgb(255, 255, 255), 1, 1, 250, 80, 2, 2, 0, 0);
            desenhar_personagem(player);
            desenhar_personagem(gato);
            al_flip_display();
            redraw = false;
        }
    }
    al_destroy_bitmap(tileset);;
    al_destroy_font(font);
    al_destroy_display(disp);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);
}
