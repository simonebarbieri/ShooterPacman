CC=gcc

pacman: main.c controllo.c fantasma.c missile.c pacman.c
	$(CC) -o pacman main.c controllo.c fantasma.c missile.c pacman.c -lncurses