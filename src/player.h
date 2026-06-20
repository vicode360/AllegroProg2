#ifndef PROG2TRABALHO_PLAYER_H
#define PROG2TRABALHO_PLAYER_H

#include <allegro5/allegro.h>

typedef struct {
    float x, y;
    int largura_frame, altura_frame;
    int direcao;
    int frame_atual;


    bool usa_sprite_unico;
    ALLEGRO_BITMAP* sprites[4]; // caso "usa_sprite_unico" for falso (varias imagens)
    ALLEGRO_BITMAP* sprite; // caso "usa_sprite_unico" for verdadeiro (uma so imagem)

    //ataque
    bool atacando;
    int timer_ataque;

} Personagem;


//inicializa um personagem q so tem um sprite
void inicializar_player_sheet(Personagem *p, float x_inicial, float y_inicial, const char* caminho_imagem);

//inicializa um personagem com sprites em png separados
void inicializar_player_sprites(Personagem *p, float x_inicial, float y_inicial, const char* caminhos[4]);

//desenha os bixo na tela
void desenhar_personagem(Personagem p);

//no allegro tem q destruir td pra otimizar a memoria
void destruir_personagem(Personagem *p);


#endif //PROG2TRABALHO_PLAYER_H
