#ifndef _xsyscalls_h
#define _xsyscalls_h

#include "super.h"
#include "inode.h"

// disco montado
int xDisc;
struct SuperBlock xReadBlock;
long int xpath = 0;

typedef struct Inode * XFILE;

// formatar disco rapidamente sem limpar os blocos
void xformatFast();
void xformat();
void xtest(int xDisc);
void xmount( char * pathDisco );
void xdismount();
void xmkdir( char * nomePasta);
XFILE * xopen( char * nomeArquivo, char * tipo);
void xwrite( XFILE arquivo, char * buffer, int tamanhoEscrita);
#endif