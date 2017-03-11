#include <stdlib.h>
#include <stdio.h>
#include <SDL/SDL.h>
#include "chip8.h"


unsigned char chip8_fontset[] =
{
	0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	0x20, 0x60, 0x20, 0x20, 0x70, // 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

void chip8_init()
{
	int i;

	chip8.PC = 0x200;
	chip8.I = 0;
	chip8.opcode = 0;
	chip8.SP = 0;

	// clear display, memory, registers, stack, keys
	for (i = 0; i < 2048; ++i)
		chip8.gfx[i] = 0;

	for (i = 0; i < 4096; ++i)
		chip8.memory[i] = 0;

	for (i = 0; i < 16; ++i)
		chip8.V[i] =  chip8.stack[i] = chip8.key[i] =  0;

	for (i = 0; i < 80; ++i)
		chip8.memory[i] = chip8_fontset[i];

	chip8.delay_timer = 0;
	chip8.sound_timer = 0;
	chip8.draw_flag = 0x01;
}

int chip8_load_game(char *filename)
{
	FILE *fp;
	long size;
	char *buffer;
	size_t r_buffer;
	int i;

	fp = fopen(filename, "rb");

	if (!fp) {
		fprintf(stderr, "Could not open game rom");
		return -ENOROM;
	}

	/* make point file go to the end */
	fseek(fp, 0, SEEK_END);
	/* get size of the file in bytes*/
	size = ftell(fp);
	printf("Size of ROM is: %ld\n", size);

	buffer = (char *) malloc (sizeof(char) * size);

	if (!buffer) {
		fprintf(stderr, "Could alloc buffer\n");
		return -ENOBUFF;
	}
	/* repos pointer file to the begin */
	fseek(fp, SEEK_SET, 0);

	/*  read byte file/rom into buffer 1 byte * size, one byte per time size
	 *  rom */
	r_buffer = fread(buffer, 1, size, fp);

	if (size != r_buffer) {
		fprintf(stderr, "Reading error\n");
		return -EREAD;
	}

	if ((M_SIZE - PG_START) > size) {
		for (i = 0; i < size; ++i)
			chip8.memory[PG_START + i] = buffer[i];
	}
	else {
		fprintf(stderr, "Rom is too big than memory\n");
		return -EBIGROM;
	}

	fclose(fp);
	free(buffer);

	return 0;
}

void chip8_cycle()
{
	;
}

void chip8_set_keys()
{
	;
}

void chip8_display()
{
}

void chip8_key_event()
{
	;
}
