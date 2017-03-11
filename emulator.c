#include <stdlib.h>
#include <stdio.h>
#include <chip8.h>

int main(int arc, char** argv)
{
	int run = 0;

	chip8_init();
	if(!chip8_load_game(argv[1]))
		printf("%d", chip8.memory[511]);

	/* main loop */
	while(!run) {
		chip8_display();
		chip8_key_event();
	}

	return 0;
}
