#ifndef POLANIE_DOS_H
#define POLANIE_DOS_H

/* dos.h on OpenWatcom contains i86.h */
#include <i86.h>

/* Overwrite fopen to our implementation */
#if !(defined(_WIN32) && !defined(__CYGWIN__)) || defined(__NGAGE__)
#ifdef __cplusplus
extern "C"
{
#endif
#include <stdio.h>
FILE	*PORT_fopen(const char *, const char *);
#define fopen(fname, mode) PORT_fopen(fname, mode)
#ifdef __cplusplus
}
#endif
#endif

#endif //POLANIE_DOS_H
