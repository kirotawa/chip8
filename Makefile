SRC:= src/*

all:
	gcc $(SRC).c -o emulator -Iinclude `pkg-config --cflags --libs sdl2`

clean:
	rm emulator
