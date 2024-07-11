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
    system("clear");
    system("rm disco.hd");
    system("dd if=/dev/zero of=disco.hd bs=MB count=100");
    printf("\n\n");

    // montar disco
    xmount("disco.hd");

    // formatar disco
    xFormat();

    // criar pastas
    xmkdir("/testando","teste.txt",0);
    xmkdir("/testando2","teste2.txt",0);
    xmkdir("/testando3/teste","teste2.txt",0);
    show_all_directories();

    find_dir(0,"testando3");
    find_dir(6,"teste");

    read_data(6);

    // criar arquivo

    // fechar disco
    xdismount();

    
}


//user_menu(xDisc);
//