#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include  <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <signal.h>
#include "xsyscalls.h"
#include "file_operations.h"
#include "super.h"
#include "directory.h"


// consultar o disco
long int physicalAddress(int blockSize, long int block){
    return block * blockSize + 1;
}
Directory* returnDirPhysicalLocation(long int block){
        long int physicalAddressNumber = physicalAddress(xReadBlock.block_size, block);
        if (physicalAddressNumber <= 1){
            physicalAddressNumber = xReadBlock.inode_directory_start;
        }
        // * Catch the father instance
        struct directory *directory = (struct directory *)malloc(xReadBlock.block_size);
        lseek(xDisc, physicalAddressNumber, SEEK_SET);
        read(xDisc, directory, xReadBlock.block_size);
        return directory;

}
Directory * find_dir(long int father_address, char *dir_name){
    
    // * Directory struct instance.
    Directory * directory_instance = returnDirPhysicalLocation(father_address);

    if (directory_instance->first_int == 0){
        printf("\n\nDiretorio > %s < não encontrado...\n\n",dir_name);
        return 0;
    }

    // * Set the first child address
    long int dir_address = physicalAddress(xReadBlock.block_size, directory_instance->first_int);

    // * Begin the searching
    long int directory_address = 0;
    long int directory_block = directory_instance->first_int;
    while (directory_address == 0){

        // * Read the directory
        lseek(xDisc, dir_address, SEEK_SET);
        read(xDisc, directory_instance, xReadBlock.block_size);
        
        if (strcmp(directory_instance->name, dir_name) == 0)
        {
            return directory_instance;
        }
        if (directory_instance->next_int == 0)
        {
            break;
        }
        dir_address = directory_instance->next_int * xReadBlock.block_size + 1;
        directory_block = directory_instance->next_int;
    }

    return 0;

}
InodeNumberNameDir * return_child_inodes(int inodeAddressFather, InodeNumberNameDir * sh_mem){
    // ! Fixed Size 
    long int * inodesNumbers = (long int *)malloc(sizeof(long int) * 64);
    long int * rootBlocks = (long int *)malloc(sizeof(long int) * 64);
    char * inodeNames[64];
    long int multiplyAddress = 0;
    long int father_address = xReadBlock.inode_directory_start;
    if (inodeAddressFather > 0){
        father_address = xReadBlock.block_size * inodeAddressFather + 1;
    }

    struct directory *directory_instance = (struct directory *)malloc(xReadBlock.block_size);
    lseek(xDisc, father_address, SEEK_SET);
    read(xDisc, directory_instance, xReadBlock.block_size);

    long int child_address = directory_instance->first_int;
    InodeNumberNameDir * inodeSet = (struct InodeNumberNameDir *) malloc(sizeof(struct InodeNumberNameDir));
     for (int i = 0; i < 64; ++i) {
        inodeSet->inodeNumbers[i] = 0; 
    }
    
    int contador = 0;

    if (child_address != 0){
        struct directory *child_instance = (struct directory *)malloc(xReadBlock.block_size);
        rootBlocks[contador] = child_address;
        lseek(xDisc, child_address * xReadBlock.block_size + 1, SEEK_SET);
        read(xDisc, child_instance, xReadBlock.block_size);
        inodesNumbers[contador] = child_instance->inode;
        inodeNames[contador]=strdup(child_instance->name);
        while (child_instance->next_int != 0){
            contador++;
            child_address = child_instance->next_int;
            rootBlocks[contador] = child_address;
            lseek(xDisc, child_instance->next_int * xReadBlock.block_size + 1, SEEK_SET);
            read(xDisc, child_instance, xReadBlock.block_size);
            inodesNumbers[contador] = child_instance->inode;
            inodeNames[contador]=strdup(child_instance->name);
        }

        for (size_t i = 0; i < contador+1; i++){
            inodeSet->inodeNumbers[i] = inodesNumbers[i];
            inodeSet->rootBlocks[i] = rootBlocks[i];
            snprintf(inodeSet->dirNames[i], sizeof(inodeNames[i]), inodeNames[i], i);
        }

    }

    return inodeSet;

}



// ferramentas
void xls(){
    
    struct directory * directory_instance = returnDirPhysicalLocation(xpath);
    printf("\n DIR: %s ---- \n",directory_instance->name);

    long int child_address = directory_instance->first_int;
    struct directory *child_instance = (struct directory *)malloc(xReadBlock.block_size);
    lseek(xDisc, child_address * xReadBlock.block_size + 1, SEEK_SET);
    read(xDisc, child_instance, xReadBlock.block_size);

    print_nexts( child_instance, child_address, 1);

}
void print_nexts( struct directory *child_instance, long int child_address, int level){
        
    for (int i = 0; i < level; i++)
    {
        printf("--");
    }

    printf("%s\n", child_instance->name);

    if (child_instance->next_int == 0 && child_instance->first_int == 0)
    {
        return;
    }

    child_address = child_instance->next_int;
    long int child_physical_address = child_address * xReadBlock.block_size + 1;

    struct directory *first = (struct directory *)malloc(xReadBlock.block_size);
    lseek(xDisc, child_instance->first_int * xReadBlock.block_size + 1, SEEK_SET);
    read(xDisc, first, xReadBlock.block_size);
    
    if (child_instance->first_int != 0)
    {
        print_nexts(first, child_address, level + 1);
    }

    lseek(xDisc, child_physical_address, SEEK_SET);
    read(xDisc, child_instance, xReadBlock.block_size);
    
    if (child_physical_address > 1){
        print_nexts(child_instance, child_address, level);
    }
}
void write_indirect(char *data, 
                    long int block_address_allocate, 
                    long int indirect_counter, 
                    long int active_indirect, 
                    int level){

    if (level == 1){
        
        write_block(data, block_address_allocate);
    
    }

    long int physical_offset = physicalAddress(xReadBlock.block_size, active_indirect)+
                                (indirect_counter*sizeof(long int));
    off_t offset = lseek(xDisc,physical_offset, SEEK_SET);
    if (offset == -1) {
        printf("Error using lseek\n");
    }

    long int *block_address_copy = (long int *)malloc(sizeof(long int));
    block_address_copy = &block_address_allocate;
    write(xDisc, block_address_copy, sizeof(long int));

    long int *block_address_read = (long int *)malloc(sizeof(long int));
    lseek(xDisc, physical_offset, SEEK_SET);
    read(xDisc, block_address_read, sizeof(long int));

}






// escrever dados em um bloco
void write_block(char *data, long int block_address){
    lseek(xDisc, physicalAddress(xReadBlock.block_size, block_address), SEEK_SET);
    write(xDisc, data, xReadBlock.block_size);
}
//ler bloco
char * read_block(long int block_address, long int i){

    long int numero;
    long int physical_offset_num = physicalAddress(xReadBlock.block_size,block_address)+(i*sizeof(long int));
    lseek(xDisc, physical_offset_num , SEEK_SET);
    read(xDisc, &numero, sizeof(long int));

    char *buffer_data = (char *)malloc(xReadBlock.block_size);
    long int physical_offset = physicalAddress(xReadBlock.block_size, numero);

    lseek(xDisc, physical_offset, SEEK_SET);
    read(xDisc, buffer_data, xReadBlock.block_size);

    return buffer_data;
}


// criar diretorio
void CreateEntry( char * dir_name, int type ){

    char *dir_path = (char *)malloc(sizeof(char) * strlen(dir_name) + 1);
    strcpy(dir_path, dir_name);

    char *token = (char *)malloc(sizeof(char) * strlen(dir_name));
    token = strtok(dir_path, "/");

    char *nomeFormatado = (char *)malloc(sizeof(char) * strlen(dir_name));

    // * We have to iterate through the path, and make sure that this path exists and the folder does not.
    long int thisDirAddress = xpath;
    long int beforeAddress = xpath;

    // * Running into path splited by /
    // ? Returning the father path in the before address

    while (token != NULL)
    {
        // * See if is the last element
        // * If it is, then we have the directory name, and the father address.
        // * If the last string has a find_dir, then the directory already exists.
        
        beforeAddress = thisDirAddress;
        thisDirAddress = find_dir( thisDirAddress, token);
        strcpy(nomeFormatado, token);

        if (thisDirAddress == 0)
        {

            // check if directory already exists
            if (find_dir( thisDirAddress, token) != 0 )
            {
                return;
            }

            if (strtok(NULL, "/") == NULL)
            {
                break;
            }
        }

        strcpy(dir_path, token);
        token = strtok(NULL, "/");
        
    }

    // * If thisDirAddress is 0, then the directory does not exist
    // * If beforeAddress is 0, then the directory is the root directory

    if (thisDirAddress == 0){
        // * Creating the directory
        struct directory * directory_instance = (struct directory *)malloc(xReadBlock.block_size);
        strcpy(directory_instance->name, nomeFormatado);

        // * Allocating in the bitmap of data

        long int free_data_block = return_free_data_bit(xDisc, xReadBlock);

        if (free_data_block == -1)
        {
            printf("No free data blocks.\n");
            return;
        }


         // * Verify if the directory is the root directory
         // * If it is, then the father block will be in the super informations
         // * Otherwise, we need do the math to find the father block
        long int physicalFatherAddress = xReadBlock.inode_directory_start;
        if (beforeAddress != 0){
            physicalFatherAddress = beforeAddress * xReadBlock.block_size + 1;
        }

        // * Catch the father instance
        struct directory *father_directory = (struct directory *)malloc(xReadBlock.block_size);
        lseek(xDisc, physicalFatherAddress, SEEK_SET);
        read(xDisc, father_directory, xReadBlock.block_size);

        // * We have two cases:
        // * 1 - The father directory does not have a first directory
        // * 2 - The father directory has a first directory

        if (father_directory->first_int == 0){
            // * If he doesn't have a first directory, then we allocate the directory in the first
            father_directory->first_int = free_data_block;
            lseek(xDisc, physicalFatherAddress, SEEK_SET);
            write(xDisc, father_directory, xReadBlock.block_size);

        } else {
            // * Else we need to iterate through the next's of the father directory
            // * And allocate the directory in the last next
            struct directory *next_directory = (struct directory *)malloc(xReadBlock.block_size);
            long int physicalNextAddress = father_directory->first_int * xReadBlock.block_size + 1;
            
            // * Catching the next directory from memory
            lseek(xDisc, physicalNextAddress, SEEK_SET);
            read(xDisc, next_directory, xReadBlock.block_size);
            
            while (next_directory->next_int != 0){
                physicalNextAddress = next_directory->next_int * xReadBlock.block_size + 1;
                lseek(xDisc, physicalNextAddress, SEEK_SET);
                read(xDisc, next_directory, xReadBlock.block_size);
            }

            // * Allocating the directory in the last next
            // * And updating the next of the last next

            next_directory->next_int = free_data_block;
            lseek(xDisc, physicalNextAddress, SEEK_SET);
            write(xDisc, next_directory, xReadBlock.block_size);
        }
        
        // alocando expaco para os indirects
        long int *indirects = (long int *)malloc(4 * sizeof(long int));
        indirects[0] = return_free_data_bit(xDisc, xReadBlock);

        // * Allocating Inode
        long int inodeInt = allocate_inode(xDisc, xReadBlock, indirects, type);
        directory_instance->inode = inodeInt;

        // * Allocating the directory in the free data block
        long int physicalDirectoryAddress = physicalAddress(xReadBlock.block_size, free_data_block);
        lseek(xDisc, physicalDirectoryAddress, SEEK_SET);
        write(xDisc, directory_instance, xReadBlock.block_size);  
    }

}
