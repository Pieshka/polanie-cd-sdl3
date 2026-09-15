#ifndef POLANIE_CONIO_H
#define POLANIE_CONIO_H

#ifdef __cplusplus
extern "C"
{
#endif

int cprintf(const char *format, ...);
int getch(void);
int kbhit(void);
unsigned ___outp(unsigned __port, unsigned __value); /* SDL also includes outp so it is conflicting... */
#define outp(port, value) ___outp(port, value)

#ifdef __cplusplus
}
#endif

#endif //POLANIE_CONIO_H
