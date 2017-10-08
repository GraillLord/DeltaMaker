# Makefile pour DeltaMaker
# Pour compiler :
#		make all
# Si ca ne marche pas, remplacer gcc par cc.

PKG = `pkg-config --cflags --libs gtk+-2.0`
CFLAGS = -Wall -Wextra -mwindows -std=gnu99
CC = gcc

O = DeltaMaker									# objets
S = Makefile main.c parseur.c parseur.h			# sources

all: $O

DeltaMaker: parseur.o main.o
			$(CC) -o DeltaMaker parseur.o main.o $(CFLAGS) $(PKG)

parseur.o: parseur.c
			$(CC) -o parseur.o -c parseur.c $(CFLAGS) $(PKG)

main.o: main.c parseur.h
			$(CC) -o main.o -c main.c $(CFLAGS) $(PKG)

clean:
	rm -rf *.exe core *.o *.output *~
