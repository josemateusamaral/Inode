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

    
    // ler arquivo e escrever no disco
    char * escrita = (char *)malloc(745);
    FILE * arquivoEntrada = fopen("assets/imagem.png","rb");
    fseek(arquivoEntrada,0,SEEK_SET);
    fread(&escrita[0],1,745,arquivoEntrada);
    
    // chamada de sistema para escrever arquivo
    xseek(&arquivo,0,SEEK_SET);
    xwrite(&arquivo, &escrita[0], 745);

    // ler dados de um arquivo
    long int tamanhoLeitura = 745;
    unsigned char * leitura = (unsigned char *)malloc(tamanhoLeitura);
    xseek(&arquivo,0,SEEK_SET);
    xread(&arquivo, &leitura[0], tamanhoLeitura);
    printf("\nLEITURA:");
    for( int i = 0 ; i < tamanhoLeitura ; i++ ){
    //    printf("%02X ",leitura[i]);
    }
    printf("\n");

    // escrever arquivo fora
    FILE * arquivoSaida = fopen("temps/saida.png","wb");
    lseek(arquivoSaida,0,SEEK_SET);
    fwrite(leitura, sizeof(unsigned char), 745, arquivoSaida);
    

    xls();
    xdismount();
    
    //xchdir("teste");
    
}
