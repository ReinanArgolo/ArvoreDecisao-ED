CC = gcc
CFLAGS = -Wall -Wextra -I./src
SRC_DIR = src
TADS_DIR = src/tads
OBJ_DIR = obj

SRCS = $(SRC_DIR)/main.c \
       $(SRC_DIR)/arvore.c \
       $(SRC_DIR)/jogador.c \
       $(SRC_DIR)/jogo.c \
       $(SRC_DIR)/utils.c \
       $(TADS_DIR)/lista.c \
       $(TADS_DIR)/pilha.c \
       $(TADS_DIR)/fila.c

OBJS = $(SRCS:.c=.o)
EXEC = codebattle

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(SRC_DIR)/*.o $(TADS_DIR)/*.o $(EXEC)
