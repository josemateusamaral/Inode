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

    // montar disco
    xmount();

    // formatar disco
    xFormat();

    // criar pasta
    xmkdir(int xDisc, struct SuperBlock ReadBlock, char *dir_name, char *file_name, long int father_address);
    show_all_directories(int xDisc, struct SuperBlock ReadBlock);

    // fechar disco
    xdismount();

    
}


//user_menu(xDisc);