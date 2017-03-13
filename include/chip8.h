#ifndef _CHIP8_H
#define _CHIP8_H

/* Error handles */
#define ENOROM  23
#define ENOBUFF 22
#define EREAD   21
#define EBIGROM 20

/* chip8 specifications */
#define M_SIZE 4096
#define PG_START 0x200 // 0x200 to 0xFFF program ROM and work RAM
#define REG_SIZE 16
#define GFX_RATIO 64*32

#define VY(byte) (byte & 0xF0) >> 4
#define VX(byte) ((byte & 0xFF00) >> 8) & 0x0F
#define OPCOND(byte) (byte & 0x0F)

struct _chip8 {
	unsigned short opcode; // 2 bytes opcode 0x0000
	unsigned char memory[M_SIZE]; // up to 4k - 4096 bytes
	unsigned char V[REG_SIZE]; // 16 registers
	unsigned char VF; // Flag register
	unsigned short I; // Index address
	unsigned short PC; // program counter
	unsigned char gfx[GFX_RATIO]; //
	unsigned char delay_timer;
	unsigned char sound_timer;
	unsigned short stack[16];
	unsigned short SP; // stack pointer
	unsigned char key[16]; // keyboard map
	unsigned char draw_flag; // 0x00 or 0x01
} chip8;

extern unsigned char chip8_fontset[];

void chip8_init(void);
int  chip8_load_game(char *);
void chip8_cycle(void);
void chip8_set_keys(void);
void chip8_display(void);
void chip8_key_event(void);
void chip8_instructions_handler(unsigned short);

#endif
