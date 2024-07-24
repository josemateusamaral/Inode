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
#include "includes/user_interaction.c"
#include <unistd.h>


void main()
{

    // criar imagem de disco
    system("clear");
    system("rm disco.hd");
    system("dd if=/dev/zero of=disco.hd bs=MB count=123");
    printf("\n\n");

    // montar disco
    xmount("disco.hd");

    // formatar disco
    xformat();

    // criar pastas
    xmkdir("testando");
    xmkdir("testando2");
    xmkdir("teste");

    xmkdir("/teste/teste2");

    // criar arquivo
    xopen("samurai.png");

    show_all_directories();
    xdismount();

    
}


//user_menu(xDisc);
//