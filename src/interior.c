#include "interior.h"

// bitmaps:
static ALLEGRO_BITMAP* chao_interior = NULL;
static ALLEGRO_BITMAP* parede_cima = NULL;
static ALLEGRO_BITMAP* parede_lateral = NULL;
static ALLEGRO_BITMAP* moveis = NULL;

static ALLEGRO_BITMAP* cama = NULL;
static ALLEGRO_BITMAP* mesa = NULL;
static ALLEGRO_BITMAP* tapete = NULL;
static ALLEGRO_BITMAP* vaso = NULL;
static ALLEGRO_BITMAP* bau = NULL;
static ALLEGRO_BITMAP* livro = NULL;

// hitbox dos moveis:

typedef struct {
    int x, y, w, h;
}Hitbox;

static const Hitbox hitbox_moveis[] = {
  {120, 110, 70, 90}, // cama
    {250, 215, 115, 50}, // mesa
    {145, 310, 20, 30}, //vaso
    {225, 120, 50, 30}, // bau
    {310, 80, 80, 100}, // livros
};
static const int num_hitboxes = 5;

void inicializar_interior() {
    chao_interior = al_load_bitmap("assets/img/InteriorChao.png");
    parede_cima = al_load_bitmap("assets/img/InteriorCima56x5.png");
    parede_lateral = al_load_bitmap("assets/img/InteriorParedeLateral8x57.png");
    moveis = al_load_bitmap("assets/img/Interior.png");

    cama = al_create_sub_bitmap(moveis, 0, 35, 80, 90); //x, y, largura, altura
    mesa = al_create_sub_bitmap(moveis, 115, 405, 115, 80);
    tapete = al_create_sub_bitmap(moveis, 0, 660, 130, 90);
    vaso = al_create_sub_bitmap(moveis, 530, 530, 50, 70);
    bau = al_create_sub_bitmap(moveis, 610, 530, 60, 70);
    livro = al_create_sub_bitmap(moveis, 500, 15, 80, 110);
}

void destruir_interior() {
    al_destroy_bitmap(cama);
    al_destroy_bitmap(mesa);
    al_destroy_bitmap(tapete);
    al_destroy_bitmap(vaso);
    al_destroy_bitmap(bau);
    al_destroy_bitmap(livro);
    al_destroy_bitmap(chao_interior);
    al_destroy_bitmap(parede_cima);
    al_destroy_bitmap(parede_lateral);
    al_destroy_bitmap(moveis);
}

void desenhar_interior(int altura_tela, int largura_tela) {
    int chao_w = al_get_bitmap_width(chao_interior);
    int chao_h = al_get_bitmap_height(chao_interior);
    int tamanho_do_chao_x = 8*chao_w;
    int tamanho_do_chao_y = 8*chao_h;
    int inicio_x = (largura_tela - tamanho_do_chao_x)/2;
    int inicio_y = (altura_tela - tamanho_do_chao_y)/2;

    // chao
    for (int linha = 0; linha < 8; linha++) {
        for (int coluna = 0; coluna < 8; coluna++) {
            int pos_x = inicio_x + coluna*chao_w;
            int pos_y = inicio_y + linha*chao_h;
            al_draw_bitmap(chao_interior, pos_x, pos_y, 0);
        }
    }
    // parede
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

    // moveis
    al_draw_bitmap(cama, 115, 110, 0);
    al_draw_bitmap(mesa, 250, 200, 0);
    al_draw_bitmap(tapete, 185, 290, 0);
    al_draw_bitmap(vaso, 120, 300, 0);
    al_draw_bitmap(bau, 225, 120, 0);
    al_draw_bitmap(livro, 310, 70, 0);
}
void debug_desenhar_hitboxes(int player_x, int player_y,
                              int off_px, int off_py,
                              int hit_pw, int hit_ph) {
    // móveis — vermelho
    ALLEGRO_COLOR cor_movel = al_map_rgba(255, 0, 0, 120);
    for (int i = 0; i < num_hitboxes; i++) {
        Hitbox h = hitbox_moveis[i];
        al_draw_filled_rectangle(h.x, h.y, h.x + h.w, h.y + h.h, cor_movel);
        al_draw_rectangle(h.x, h.y, h.x + h.w, h.y + h.h, al_map_rgb(255,0,0), 1);
    }
    // hitbox do player — azul
    int px = player_x + off_px;
    int py = player_y + off_py;
    al_draw_filled_rectangle(px, py, px + hit_pw, py + hit_ph, al_map_rgba(0, 0, 255, 120));
    al_draw_rectangle(px, py, px + hit_pw, py + hit_ph, al_map_rgb(0, 0, 255), 1);
}

bool colisao_moveis(int next_x, int next_y, int off_px, int off_py, int hit_pw, int hit_ph) {
    int px = next_x + off_px;
    int py = next_y + off_py;
    for (int i = 0; i < num_hitboxes; i++) {
        Hitbox h = hitbox_moveis[i];
        if (px + hit_pw > h.x && px < h.x + h.w && py + hit_ph > h.y && py < h.y + h.h) {
            return true;
        }
    }
return false;
}

bool colisao_parede(int x, int y, int direcao) {
    if (x < 85 && direcao == 2) return true; // parede esquerda
    if (x > 300 && direcao == 3) return true; // parede direita
    if (y < 100 && direcao == 1) return true; // parede cim a
    if (y > 295 && direcao == 0 && (x < 170 || x > 210)) return true;
    return false;
}