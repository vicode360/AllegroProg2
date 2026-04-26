#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/keyboard.h>
#include <stdbool.h>
#include <stdio.h>
#include "mapa.h"

// largura - 160
// altura - 152
// testesss

int main()
{
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
    ALLEGRO_BITMAP* boneco[4];
    boneco[0] = al_load_bitmap("assets/img/run_down.png"); // 96 largura
    boneco[1] = al_load_bitmap("assets/img/run_up.png");   // 80 altura
    boneco[2] = al_load_bitmap("assets/img/run_left.png");
    boneco[3] = al_load_bitmap("assets/img/run_right.png");

    ALLEGRO_BITMAP *casa = al_load_bitmap("assets/img/casa.png");
    ALLEGRO_BITMAP *tileset = al_load_bitmap("assets/img/TX Tileset Grass.png");
    ALLEGRO_BITMAP* sprite = al_load_bitmap("assets/img/nekos.png");
    if (!sprite) {
        fprintf(stderr, "n foi possivel carregar o arquivo, verifique se ele esta na pasta cmake-build-debug\n");
        return -1;
    }


    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));

    float frame = 0.f;
    int pos_x = 0, pos_y = 0;
    int current_frame_y = 152;
    int direcao = 0;
    int offset_gato_x = 60;
    int offset_gato_y = 85;

    bool redraw = true;
    ALLEGRO_EVENT event;

    al_start_timer(timer);
    while(1)
    {
        al_wait_for_event(queue, &event);

        if(event.type == ALLEGRO_EVENT_TIMER)
            redraw = true;
        frame += 0.2;
        if (frame > 4) {
            frame -= 4;
        }
        else if((event.type == ALLEGRO_EVENT_DISPLAY_CLOSE))
            break;
        else if(event.keyboard.keycode == ALLEGRO_KEY_RIGHT) {
            direcao = 3;
            current_frame_y = 160;
            pos_x += 6;
        }
        else if(event.keyboard.keycode == ALLEGRO_KEY_LEFT) {
            direcao = 2;
            current_frame_y = 160*3;
            pos_x -= 6;
        }
        else if(event.keyboard.keycode == ALLEGRO_KEY_UP) {
            direcao = 1;
            current_frame_y = 160*2;
            pos_y -= 6;
        }
        else if(event.keyboard.keycode == ALLEGRO_KEY_DOWN) {
            direcao = 0;
            current_frame_y = 0;
            pos_y += 6;
        }

        if(redraw && al_is_event_queue_empty(queue))
        {
            al_clear_to_color(al_map_rgb(0, 0, 0));
            desenhar_mapa(tileset);
            al_rest(0.01);
            al_draw_tinted_scaled_rotated_bitmap_region(casa, 0, 0, 80, 97, al_map_rgb(255, 255, 255), 1, 1, 250, 80, 2, 2, 0, 0);
            al_draw_tinted_scaled_rotated_bitmap_region(boneco[direcao], 96*(int)frame, -10, 96, 80, al_map_rgb(255, 255, 255), 1, 1, pos_x, pos_y, 1.25, 1.25, 0, 0);
            al_draw_tinted_scaled_rotated_bitmap_region(sprite, 160*(int)frame, current_frame_y, 160, 145, al_map_rgb(255, 255, 255), 1, 1, pos_x + offset_gato_x, pos_y + offset_gato_y, 0.2, 0.2, 0, 0);
            al_flip_display();
            redraw = false;
        }
    }
    al_destroy_bitmap(tileset);
    al_destroy_bitmap(sprite);
    al_destroy_font(font);
    al_destroy_display(disp);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);

    return 0;
}
