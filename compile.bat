@echo off
echo Compilando CodeBattle...
if not exist obj mkdir obj

gcc -Wall -Wextra -I./src -c src/main.c -o obj/main.o
gcc -Wall -Wextra -I./src -c src/arvore.c -o obj/arvore.o
gcc -Wall -Wextra -I./src -c src/jogador.c -o obj/jogador.o
gcc -Wall -Wextra -I./src -c src/jogo.c -o obj/jogo.o
gcc -Wall -Wextra -I./src -c src/utils.c -o obj/utils.o
gcc -Wall -Wextra -I./src -c src/ui.c -o obj/ui.o
gcc -Wall -Wextra -I./src -c src/tads/lista.c -o obj/lista.o
gcc -Wall -Wextra -I./src -c src/tads/pilha.c -o obj/pilha.o
gcc -Wall -Wextra -I./src -c src/tads/fila.c -o obj/fila.o

gcc -o codebattle.exe obj/main.o obj/arvore.o obj/jogador.o obj/jogo.o obj/utils.o obj/ui.o obj/lista.o obj/pilha.o obj/fila.o

echo Compilacao concluida! Execute codebattle.exe
pause
