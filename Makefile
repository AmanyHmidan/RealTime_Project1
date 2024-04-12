CC = gcc
LIBSPARENT = -lGL -lGLU -lglut 
LIBSLAST = -lglut -lGLU -lGL
LIBSPLAYER = -lm

all: Parent player last

Parent: Parent.c
	$(CC)  -o Parent Parent.c $(LIBSPARENT)

player: player.c
	$(CC)  -o player player.c $(LIBSPLAYER)
	
last: last.c
	$(CC)  -o last last.c $(LIBSLAST)
       

clean:
	rm -f Parent player last
	
