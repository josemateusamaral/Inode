#ifndef _xsyscalls_h
#define _xsyscalls_h

#include "super.h"

// disco montado
int xDisc;
struct SuperBlock xReadBlock;
long int xpath = 0;

// formatar disco rapidamente sem limpar os blocos
void xformatFast();
void xformat();
void xtest(int xDisc);
void xmount( char * pathDisco );
void xdismount();
void xmkdir( char * nomePasta);
void xopen( char * nomeArquivo);

#endif