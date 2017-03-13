#include <stdlib.h>
#include <stdio.h>
#include <chip8.h>

int main(int arc, char** argv)
{
	int run = 0;

	chip8_init();
	/*  check if rom was load properly */
	if(!!chip8_load_game(argv[1])) {
		fprintf(stderr, "Program start wasn't load\n");
		fprintf(stderr, "Start program equal %x",
				chip8.memory[PG_START + 1]);
	}

	/* main loop */
	while(!run) {
		chip8_cycle();
		chip8_display();
		chip8_key_event();
	}

	return 0;
}
