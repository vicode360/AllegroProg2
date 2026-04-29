CC = gcc
CFLAGS = $(shell pkg-config --cflags allegro-5 allegro_main-5 allegro_font-5 allegro_image-5 allegro_primitives-5)
LIBS   = $(shell pkg-config --libs   allegro-5 allegro_main-5 allegro_font-5 allegro_image-5 allegro_primitives-5)

SRCS = src/main.c src/jogo.c src/mapa.c src/player.c
OUT  = jogo

all:
	$(CC) $(SRCS) $(CFLAGS) $(LIBS) -o $(OUT)
	cp -r assets $(dir $(OUT))

clean:
	rm -f $(OUT)