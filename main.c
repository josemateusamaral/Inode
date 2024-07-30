#include <math.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "includes/super.c"
#include "includes/bitmap.c"
#include "includes/directory.c"
#include "includes/inode.c"
#include "includes/xsyscalls.c"
#include "includes/file_operations.c"
#include <unistd.h>


void main()
{

    // criar imagem de disco
    system("clear");
    system("rm disco.hd");
    system("dd if=/dev/zero of=disco.hd bs=MB count=16");
    printf("\n\n");

    // montar disco
    xmount("disco.hd");

    // formatar disco
    xformat();

    // criar pastas
    xmkdir("testando");
    xmkdir("testando2");
    xmkdir("teste");
    
    // criar arquivo
    XFILE arquivo = xopen("samurai.png","wb");
    printf("\nARQUIVO:\n");
    
    // ler arquivo e escrever no disco
    char * escrita = (char *)malloc(745);
    FILE * arquivoEntrada = fopen("assets/imagem.png","rb");
    fseek(arquivoEntrada,0,SEEK_SET);
    fread(&escrita[0],1,745,arquivoEntrada);
    
    xwrite(arquivo, escrita, 745);

    // ler dados de um arquivo
    char * leitura = (char *)malloc(745);
    xread(arquivo, leitura, 745);
    printf("\nLEITURA:");
    for( int i = 0 ; i < 745 ; i++ ){
        printf("%c",leitura[i]);
    }

    // escrever arquivo fora
    FILE * arquivoSaida = fopen("temps/saida.png","wb");
    fwrite(leitura, sizeof(unsigned char), 745, arquivoSaida);

    xchdir("teste");

    xls();

    xdismount();
    
}
