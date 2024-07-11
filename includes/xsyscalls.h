#ifndef _xsyscalls_h
#define _xsyscalls_h

#include "super.h"

// disco montado
int xDisc;
struct SuperBlock xReadBlock;

// formatar disco rapidamente sem limpar os blocos
void xFormatFast();
void xFormat();
void xtest(int xDisc);
void xmount( char * pathDisco );
void xdismount();
void xmkdir( char *dir_name, char *file_name, long int father_address);

#endif