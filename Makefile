all:
	gcc emulator.c chip8.c -o emulator -Iinclude `pkg-config --cflags --libs sdl2`

clean:
	rm emulator
