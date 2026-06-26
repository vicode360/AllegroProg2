PKGS = allegro-5 allegro_main-5 allegro_font-5 allegro_image-5 allegro_primitives-5 libcurl libcjson
CC = gcc
CFLAGS = $(shell pkg-config --cflags $(PKGS))
LIBS   = $(shell pkg-config --libs   $(PKGS))

# Windows: o pkg-config do MSYS2 gera .exe automaticamente, mas garantimos
ifeq ($(OS),Windows_NT)
    OUT = jogo.exe
    EXT = .exe
else
    OUT = jogo
    EXT =
endif

SRCS = $(wildcard src/*.c)

all: $(OUT)
$(OUT): $(SRCS)
	$(CC) $(SRCS) $(CFLAGS) $(LIBS) -o $(OUT)
	-cp -r assets ./ 2>/dev/null

clean:
	rm -f $(OUT)

.PHONY: all clean
