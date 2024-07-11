#include "directory.h"

// criar diretorio
Directory *create_dir(char *_name, int _inode)
{
    Directory *dir = (Directory *)malloc(sizeof(Directory));
    strcpy(dir->name, _name);
    dir->inode = _inode;
    dir->first_int = 0;
    dir->next_int = 0;
    return dir;
}

void printDirectory(Directory * diretorio){
    printf("Directory name: %s\n", diretorio->name);
    printf("Inode: %d\n",diretorio->inode);
    printf("First_Int: %d\n",diretorio->inode);
    printf("Next_Int: %d\n",diretorio->next_int);
	printf("--------First_Structure-------\n");
    if( diretorio->first_structure != NULL){
        printDirectory(diretorio->first_structure);
    }else{
        printf("First_Structs esta vazia...\n");
    }
    printf("------------------------------\n");
    printf("--------Next_Structure-------\n");
    if( diretorio->next_structure != NULL){
        printDirectory(diretorio->next_structure);
    }else{
        printf("Next_Structs esta vazia...\n");
    }
    printf("------------------------------\n");
}
