#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//https://en.wikipedia.org/wiki/CHIP-8#Virtual_machine_description

#define INITIAL_ADDR 0x200

int load_rom(char *path);
long file_get_size(char *path);
void interpret(unsigned short);
void disp_clear();

void jump(unsigned short);
void ret();

unsigned char mem[4096];
unsigned char V[16]; //15 general purpose registers + 1 flag register
unsigned short I; //16 bit pointer
unsigned short stack[16];
unsigned short sp = 0; //stack pointer
unsigned char screen[64 * 32]; //64x32 pixels
unsigned short PC = INITIAL_ADDR; //program counter, starting from 512
unsigned short opcode;


int main()
{
    long rom_size = load_rom("roms/Cave.ch8");
    if (rom_size <= 0) printf("Error loading rom\n");

    /*hexdump rom for debug purpose*/
    for (int i = INITIAL_ADDR; i < INITIAL_ADDR + rom_size; i+=2) {
        opcode = mem[i] << 8 | mem[i+1];
        printf("%hx ", opcode);
    }
    printf("\n");

    /*CYCLE STARTS*/
    for (;;) {
        opcode = mem[PC] << 8 | mem[PC+1];
        PC += 2; //1 opcode consists of 2 bytes

        interpret(opcode);

        if (PC == INITIAL_ADDR + rom_size) break;
    }

    return 0;
}


void interpret(unsigned short opcode)
{
    static unsigned short op1, op2, op3, op4; //first 4 bits and last 4 bits of opcode

    switch (opcode) { //static opcodes
        case 0xee:
            ret(); //return from subroutine
            return;
        case 0xe0:
            disp_clear();
            return;
    }


    /*16bit value so no clever bitshift algo*/
    op1 = opcode & 0xf000;
    op2 = opcode & 0x0f00;
    op3 = opcode & 0x00f0;
    op4 = opcode & 0x000f;
    printf("%hx:", opcode);
    switch (op1) {
        case 0x1000:
            jump(opcode & 0x0fff);
        case 0x2000: //call subroutine
            stack[sp] = PC;
            sp++;
            jump(opcode & 0x0fff);
        case 0x3000: //3XNN skip the next instruction if VX equals NN
            if (V[opcode & 0x0f00] == opcode & 0x00ff) PC += 2;
        case 0x4000: //4XNN skip the next instruction if VX equals NN
            if (V[opcode & 0x0f00] != opcode & 0x00ff) PC += 2;
        case 0x5000: //5XY0 skip the next instruction if VX equals VY
            if (V[opcode & 0x0f00] == V[opcode & 0x00f0]) PC += 2;
        case 0x6000: //6XNN set VX = NN
            V[opcode & 0x0f00] = opcode & 0x00ff;
        case 0x7000: //7XNN add NN to VX
            V[opcode & 0x0f00] += opcode & 0x00ff;
        case 0x8000:
            switch (op4) {
                case 0: //0x8XY0 set VX = VY
                    V[opcode & 0x0f00] = V[opcode & 0x00f0];
                case 1: //0x8XY1 BitOR VX |= VY
                    V[opcode & 0x0f00] |= V[opcode & 0x00f0];
                case 2: //0x8XY2 BitAND VX &= VY
                    V[opcode & 0x0f00] &= V[opcode & 0x00f0];
                case 3: //0x8XY3 BitXOR VX ^= VY
                    V[opcode & 0x0f00] ^= V[opcode & 0x00f0];
                case 4: //0x8XY4 add VX += VY
                    V[opcode & 0x0f00] += V[opcode & 0x00f0];
                case 5: //0x8XY4 substract VX -= VY
                    V[opcode & 0x0f00] -= V[opcode & 0x00f0];
                case 6: //0x8XY6 bitshift-right VX >>= 1
                    V[opcode & 0x0f00] >>= 1;
                case 7: //0x8XY7 VX = VY - VX
                    V[opcode & 0x0f00] = V[opcode & 0x00f0] - V[opcode & 0x0f00];
                case 0xe:
                    V[opcode & 0x0f00] <<= 1;
            }
        case 0x9000: //if(VX!=VY)
            if (V[opcode & 0x0f00] != V[opcode & 0x00f0]) PC += 2;
        case 0xa000: //set pointer I = NNN
            I = opcode & 0x0fff;
        case 0xb000: //PC = V0 + NNN
            PC = V[0] + opcode & 0x0fff;
        case 0xc000:
            V[opcode & 0x0f00] = random_byte() & (opcode & 0x00ff);
        case 0xd000:
            draw(opcode & 0x0f00, opcode & 0x00f0, opcode & 0x000f);
        case 0xe000:
            switch (op4) {
                case 0xe:
                    if (V[opcode & 0x0f00] == key()) PC += 2;
                case 0x1:
                    if (V[opcode & 0x0f00] != key()) PC += 2;
            }
        case 0xf000:
            switch (opcode & 0x00ff) {
                case 0x07:
                    V[opcode & 0x0f00] = get_delay();
                case 0x0a:
                    V[opcode & 0x0f00] = get_key();
                case 0x15:
                    delay_timer(V[opcode & 0x0f00]);
                case 0x18:
                    sound_timer(V[opcode & 0x0f00]);
                case 0x1e:
                    I += V[opcode & 0x0f00];
                /*case 0x29:
                    I = sprite_addr[V[opcode & 0x0f00]];*/
                case 0x33:
                    set_BCD(V[opcode & 0x0f00]);
                    mem[I] = BCD(3);
                    mem[I+1] = BCD(2);
                    mem[I+2] = BCD(1);
                case 0x55:
                    reg_dump(V[opcode & 0x0f00], I);
                case 0x65:
                    reg_load(V[opcode & 0x0f00], I);
            }

    }

}


//draw a sprite at coordinate X,Y with 8 pixels width and N pixels height
void disp_draw(unsigned char X, unsigned char Y, unsigned char N)
{
    //TODO: write to screen[] array, then call SDL lib to draw to actual screen.
}


void disp_clear() //clear the screen
{
    memset(screen, 0, sizeof(screen));
}


void jump(unsigned short addr) //addr must always be even, though no check here to increase performance
{
    printf("Jumping to %hx\n", addr);
    PC = addr;
}


void ret()
{
    /*pop saved mem addr from stack and jump to it*/
    jump(stack[sp]);
    sp--;
}


unsigned char random_byte()
{
    return (unsigned char)random() % 256; //return random value between 0-255
}


int load_rom(char *path)
{
    long fsize = file_get_size(path);
    if (fsize > sizeof(mem) - INITIAL_ADDR) return -1; //rom size too large

    FILE *rom_file = fopen(path, "rb");
    if (rom_file == NULL) return -2;

    fread(mem + INITIAL_ADDR, fsize, 1, rom_file);
    fclose(rom_file);
    
    return fsize;
}


long file_get_size(char *path)
{
    FILE *fd = fopen(path, "r");
    if (!fd) return 0;
    fseek(fd, 0L, SEEK_END);
    long ret = ftell(fd);
    fclose(fd);
    
    return ret;
}