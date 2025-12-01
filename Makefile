# Simple cross-platform Makefile (expects GCC/Clang)

CC ?= gcc
CFLAGS ?= -std=c11 -O2 -Wall -Wextra

SRC := src/main.c src/tree.c src/game.c
INC := -Iinclude
OUT := bin/jogo

OS := $(shell uname 2>/dev/null || echo Windows)
ifeq ($(OS),Windows)
	OUT := bin/jogo.exe
endif

$(OUT): $(SRC)
	@mkdir -p bin
	$(CC) $(CFLAGS) $(INC) $(SRC) -o $(OUT)

.PHONY: clean run
clean:
	$(RM) -r bin

run: $(OUT)
	./$(OUT)
