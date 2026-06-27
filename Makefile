CC = gcc
SRCS = $(wildcard src/*.c)

# --- detecao de plataforma ---
ifeq ($(OS),Windows_NT)
    OUT = jogo.exe
else
    OUT = jogo
endif

# --- flags de compilacao ---
# Tenta pkg-config primeiro (Linux/macOS/MSYS2). Se nao houver pkg-config,
# cai no fallback manual (Windows sem MSYS2, vcpkg, etc).
PKGS = allegro-5 allegro_main-5 allegro_font-5 allegro_image-5 allegro_primitives-5 libcurl libcjson

PKG_CONFIG := $(shell command -v pkg-config 2>/dev/null)

ifdef PKG_CONFIG
    CFLAGS = $(shell pkg-config --cflags $(PKGS))
    LIBS   = $(shell pkg-config --libs   $(PKGS))
else
    # Fallback sem pkg-config: assume que as libs estao no PATH/include/lib padrao
    CFLAGS =
    LIBS   = -lallegro -lallegro_main -lallegro_font -lallegro_image -lallegro_primitives -lcurl -lcjson
endif

# --- regras ---
all: $(OUT)

$(OUT): $(SRCS)
	$(CC) $(SRCS) $(CFLAGS) $(LIBS) -o $(OUT)
	-cp -r assets ./ 2>/dev/null

clean:
	rm -f $(OUT)

.PHONY: all clean