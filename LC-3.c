#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdint.h>

enum
{
    R_R0 = 0,
    R_R1,
    R_2,
    R_R3,
    R_R4,
    R_R5,
    R_R6,
    R_R7,
    R_PC,
    R_COND,
    R_COUNT
};

enum
{
    OP_BR = 0, /* branch */
    OP_ADD,    /* add  */
    OP_LD,     /* load */
    OP_ST,     /* store */
    OP_JSR,    /* jump register */
    OP_AND,    /* bitwise and */
    OP_LDR,    /* load register */
    OP_STR,    /* store register */
    OP_RTI,    /* unused */
    OP_NOT,    /* bitwise not */
    OP_LDI,    /* load indirect */
    OP_STI,    /* store indirect */
    OP_JMP,    /* jump */
    OP_RES,    /* reserved (unused) */
    OP_LEA,    /* load effective address */
    OP_TRAP    /* execute trap */
};

enum
{
    FL_POS = 1 << 0, /* P */
    FL_ZRO = 1 << 1, /* Z */
    FL_NEG = 1 << 2, /* N */
};

enum
{
    TRAP_GETC = 0x20,  /* get character from keyboard, not echoed onto the terminal */
    TRAP_OUT = 0x21,   /* output a character */
    TRAP_PUTS = 0x22,  /* output a word string */
    TRAP_IN = 0x23,    /* get character from keyboard, echoed onto the terminal */
    TRAP_PUTSP = 0x24, /* output a byte string */
    TRAP_HALT = 0x25   /* halt the program */
};

uint16_t reg[R_COUNT];
uint16_t memory[UINT16_MAX];

int int main(int argc, char const *argv[])
{
    enum{PC_START = 0x3000};
    reg[R_PC] = PC_START;

    int running = 1;
    while(running)
    {
        uint16_t instr = mem_read(reg[R_PC]++);
        uint16_t op = instr >> 12;
        switch(op)
        {
            case OP_BR:
            break;
            case OP_ADD:
            {ADD,6}
            break;
            case OP_LD:
            break;
            case OP_ST:
            break;
            case OP_JSR:
            break
        }
    }
    return 0;
}

void mem_set(uint16_t address,uint16_t data)
{
    memory[address] = data;
}

uint16_t mem_read(uint16_t address)
{
    return memory[address];
}

uint16_t sign_extend(uint16_t x,int bit_count)
{
    if(x >> (bit_count - 1) & 1){
        x |= (0xFFFF << bit_count);
    }
    return x;
}

void update_flags(uint16_t r)
{
    if(reg[r] == 0)
    {
        reg[R_COND] = FL_ZRO;
    }
    else if(reg[r] >> 15)
    {
        reg[R_COND] = FL_NEG;
    }
    else
    {
        reg[R_COND] = FL_POS;
    }
}

void add(uint16_t instr)
{
    uint16_t r0 = (instr >> 9) & 0x7;
    uint16_t r1 = (instr >> 6) & 0x7;
    uint16_t imm_flag = (instr >> 5) & 0x1;
    if(imm_flag)
    {
        uint16_t imm5 = sign_extend(instr & 0x1F,5);
        reg[r0] = reg[r1] + imm5;
    }
    else
    {
        uint16_t r2 = instr & 0x7;
        reg[r0] = reg[r1] + reg[r2];
    }
    update_flags(r0);
}

void and(uint16_t instr)
{
    uint16_t dr = (instr >> 9) & 0x7;
    uint16_t sr1 = (instr >> 6) & 0x7;
    uint16_t imm_flag = (instr >> 5) & 0x1;
    if(imm_flag)
    {
        uint16_t imm5 = sign_extend(instr & 0x1F,5);
        reg[dr] = reg[sr1] & imm5;
    }
    else
    {
        uint16_t sr2 = instr & 0x7;
        reg[dr] = reg[sr1] & reg[sr2];
    }
    update_flags(dr);
}

void br(uint16_t instr)
{
    uint16_t offset = sign_extend(instr & 0x1FF,9);
    uint16_t flag = (instr >> 9) & 0x7;
    if(flag & reg[R_COND])
    {
        reg[R_PC] += offset;
    }
}

void jmp(uint16_t instr)
{
    uint16_t base_r = (instr >> 6) & 0x7;
    reg[R_PC] = reg[base_r];
}

void ret()
{

}

void jsr(uint16_t instr)
{

}

void ld(uint16_t instr)
{
    uint16_t offset = instr & 0x1FF;
    uint16_t dr = (instr >> 9) & 0x7;
    reg[dr] = mem_read(reg[R_PC] + sign_extend(offset,9))
    update_flags(dr);
}

void ldi(uint16_t instr)
{
    uint16_t offset = instr & 0x1FF;
    uint16_t dr = (instr >> 9) & 0x7;
    reg[dr] = mem_read(mem_read(reg[R_PC] + sign_extend(offset,9)));
    update_flags(dr);
}

void ldr(uint16_t instr)
{
    uint16_t offset = instr & 0x3F;
    uint16_t base_r = (instr >> 6) & 0x7;
    uint16_t dr = (instr >> 9) & 0x7;
    reg[dr] = mem_read(reg[base_r] + sign_extend(offset,6));
    update_flags(dr);
}

void lea(uint16_t instr)
{
    uint16_t offset = instr & 0x1FF;
    uint16_t dr = (instr >> 9) & 0x7;
    reg[dr] = reg[R_PC] + sign_extend(offset,9);
    update_flags(dr);
}

void not(uint16_t instr)//ERROR?????
{
    uint16_t sr = (instr >> 6) & 0x7;
    uint16_t de = (instr >> 9) & 0x7;
    reg[dr] = ~reg[sr];
    update_flags(dr);
}

void st(uint16_t instr)
{
    uint16_t offset = instr & 0x1FF;
    uint16_t sr = (instr >> 9) & 0x7;
    mem_set(reg[R_PC] + sign_extend(offset,9),reg[sr]);
}

void sti(uint16_t instr)
{
    uint16_t offset = instr & 0x1FF;
    uint16_t sr = (instr >> 9) & 0x7;
    mem_set(mem_read(reg[R_PC] + sign_extend(offset,9)),reg[sr]);
}

void str(uint16_t instr)
{
    uint16_t offset = instr & 0x3F;
    uint16_t base_r = (instr >> 6) & 0x7;
    uint16_t sr = (instr >> 9) & 0x7;
    mem_set(mem_read(reg[base_r] + sign_extend(offset,6)),reg[sr]);
}