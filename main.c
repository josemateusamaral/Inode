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

/**
Criar HD = 
*/


void main()
{

    // montar disco
    xmount();

    // formatar disco
    xFormatFast();

    // fechar disco
    xdismount();

    
}


//user_menu(xDisc);