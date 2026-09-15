#ifndef POLANIE_I86_H
#define POLANIE_I86_H

#ifdef __cplusplus
extern "C"
{
#endif

#pragma pack(push, 1)

struct DWORDREGS
{
    unsigned int eax;
    unsigned int ebx;
    unsigned int ecx;
    unsigned int edx;
    unsigned int esi;
    unsigned int edi;
    unsigned int cflag;
};

struct WORDREGS
{
    unsigned short ax;
    unsigned short bx;
    unsigned short cx;
    unsigned short dx;
    unsigned short si;
    unsigned short di;
    unsigned short cflag;
};

struct BYTEREGS
{
    unsigned char al, ah;
    unsigned char bl, bh;
    unsigned char cl, ch;
    unsigned char dl, dh;
};

union REGS
{
    struct DWORDREGS x;
    struct WORDREGS w;
    struct BYTEREGS h;
};

struct SREGS
{
    unsigned short es, cs, ss, ds;
    unsigned short fs, gs;
};
#define _SREGS SREGS

void delay(unsigned int __miliseconds);
int int386(int, union REGS*, union REGS* );

/* Virtual Functions, not in the original i86.h */
void* PORT_getFakeFramebuffer();
void PORT_SDLPumpEvents(int only_pump);

#pragma pack(pop)

#ifdef __cplusplus
}
#endif

#endif //POLANIE_I86_H
