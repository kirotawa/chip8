#include <stdlib.h>
#include <stdio.h>
#include <time.h>
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

/* in each cycle emulator walk PC + 2 getting instructions opcode */
void chip8_cycle()
{
	chip8.opcode = chip8.memory[chip8.PC] << 8 | chip8.memory[chip8.PC + 1];
	chip8.PC += 2;
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

/* This function handles with all opcode. It's called in every cycle */
void chip8_instructions_handler(unsigned short opcode)
{
	unsigned char b1;
	unsigned char b2;
	unsigned char aux1;
	unsigned char aux2;

	b1 = (opcode & 0xF000) >> 8;
	b2 = (opcode & 0x00FF);

	aux1 = b1;
	aux2 = b2;
	switch (b1) {
		case 0x00:
			/* Return instruction */
			if (b2 == 0xEE)
				chip8.PC = chip8.stack[chip8.SP];
			if (b2 == 0xE0)
				;
			break;

		case 0x10:
			/* goto instructions, jump to the NNN address */
			chip8.PC = opcode & 0x0FFF;
			break;

		case 0x20:
			/*  calls subroutine in NNN */
			chip8.stack[chip8.SP] = chip8.PC;
			chip8.PC = opcode & 0x0FFF;
			break;

		case 0x30:
			/* Skip if Vx == NN */
			b1 = (opcode & 0xFF00) >> 8;
			if (chip8.V[b1 & 0x0F] == b2)
				chip8.PC += 2;
			break;

		case 0x40:
			/* Skip if Vx != NN */
			b1 = (opcode & 0xFF00) >> 8;
			if (chip8.V[b1 & 0x0F] != b2)
				chip8.PC += 2;
			break;

		case 0x50:
			/* Skip if Vx == Vy */
			b1 = (opcode & 0xFF00) >> 8;
			if (chip8.V[b1 & 0x0F] == chip8.V[VY(b2)])
				chip8.PC += 2;
			break;

		case 0x60
			/* Sets Vx to NN */:
			b1 = (opcode & 0xFF00) >> 8;
			chip8.V[b1 & 0x0F] = b2;
			break;

		case 0x70:
			/*  Adds NN to Vx  */
			b1 = (opcode & 0xFF00) >> 8;
			chip8.V[b1 & 0x0F] += b2;
			break;

		case 0x80:
			aux1 = VX(opcode);
			aux2 = VY(aux2);
			/* Sets Vx to value of Vy */
			if (OPCOND(b2) == 0x00) {
				chip8.V[aux1] = chip8.V[aux2];
			}

			/* A bitwise OR between Vx | Vy */
			if (OPCOND(b2) == 0x01) {
				chip8.V[aux1] = chip8.V[aux1] | chip8.V[aux2];
			}

			/*  A bitwise AND Vx & Vy */
			if (OPCOND(b2) == 0x02) {
				chip8.V[aux1] = chip8.V[aux1] & chip8.V[aux2];
			}

			/* A XOR operation Vx ^ Vy */
			if (OPCOND(b2) == 0x03) {
				chip8.V[aux1] = chip8.V[aux1] ^ chip8.V[aux2];

			}

			/* Add Vy to Vx and set VF flag if there's a carry */
			if (OPCOND(b2) == 0x04) {
				if ((chip8.V[aux1] += chip8.V[aux2]) > 0xFF)
					chip8.VF = 1;
				else
					chip8.VF = 0;
				chip8.V[aux1] += chip8.V[aux2];
			}

			/* Sub Vy to Vx and set VF flag if there's a borrow */
			if (OPCOND(b2) == 0x05) {
				if (chip8.V[aux2] > chip8.V[aux1])
					chip8.VF = 0;
				else
					chip8.VF = 1;

				chip8.V[aux1] -= chip8.V[aux2];
			}

			/* Shift Vx by 1 and set the least significant bit to VF */
			if (OPCOND(b2) == 0x06) {
				chip8.VF = chip8.V[aux1] & 0x0001;
				chip8.V[aux1] = chip8.V[aux1] >> 1;
			}

			/* Sub Vy - Vx and assign the result to Vx */
			if (OPCOND(b2) == 0x07) {
				if (chip8.V[aux1] > chip8.V[aux2])
					chip8.VF = 1;
				else
					chip8.VF = 0;

				chip8.V[aux1] = chip8.V[aux2] - chip8.V[aux1];
			}

			/* Shift left by 1 Vx register */
			if (OPCOND(b2) == 0x0E) {
				chip8.VF = (chip8.V[aux1] & 0xF0) >> 7;
				chip8.V[aux1] = chip8.V[aux1] << 1;
			}
			break;

		case 0x90:
			aux1 = VX(opcode);
			aux2 = VY(aux2);

			if (chip8.V[aux1] != chip8.V[aux2])
				chip8.PC += 2;
			break;

		case 0xA0:
			chip8.I = opcode & 0x0FFF;
			break;

		case 0XB0:
			chip8.PC = chip8.V[0] + (opcode & 0x0FFF);
			break;

		case 0xC0:
			aux1 = VX(opcode);
			srand(time(NULL));
			aux2 = rand() % 256;
			chip8.V[aux1] = aux2 & (opcode & 0x0FFF);

			break;

		case 0xD0:
			/* draws routines */
			break;

		case 0xE0:
		case 0xF0:

		default:
			break;
	}
}
