
CC = gcc
CFLAGS = -I/opt/homebrew/include -L/opt/homebrew/lib -lraylib -lm -lpthread -pedantic  -Wextra -Wall

all: main

main: main.c map_nodes.c world.c
	$(CC) main.c map_editor.c map_nodes.c map_sectors.c map_utils.c player.c world.c client.c server.c -o main $(CFLAGS)

clean:
	rm -f main
