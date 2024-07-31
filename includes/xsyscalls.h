#ifndef _xsyscalls_h
#define _xsyscalls_h

#include "super.h"
#include "inode.h"

// disco montado
int xDisc;
struct SuperBlock xReadBlock;
long int xpath = 0;

struct XFILE{
    struct Inode * inode;
    long int posicaoRelativa;
    long int posicaoFisica;
    long int tamanhoArquivo;
};
typedef struct XFILE XFILE;
void printXFILE(XFILE * arquivo);

// formatar disco rapidamente sem limpar os blocos
void xformatFast();
void xformat();
void xtest(int xDisc);
void xmount( char * pathDisco );
void xdismount();
void xmkdir( char * nomePasta);
XFILE xopen( char * nomeArquivo, char * tipo);
void xwrite( XFILE * arquivo, char * buffer, int tamanhoEscrita);
void xread( XFILE * arquivo, char * dadosLidos, int tamanhoLeitura);
void xseek( XFILE * file, long int offset, int posicionador);
void xchdir(char * dir_name);

#endif