CC = gcc
SRC = orbit.c
OUT = orbit

CFLAGS = `sdl2-config --cflags`
LDFLAGS = `sdl2-config --libs` -lSDL2_image

.PHONY: orbit clean

orbit: $(SRC)
	$(CC) $(CFLAGS) -o $(OUT) $(SRC) $(LDFLAGS)

clean:
	rm -f $(OUT)