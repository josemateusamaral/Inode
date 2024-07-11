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


// calcular posicao do bloco no disco
long int physicalAddress(int blockSize, long int block){
    return block * blockSize + 1;
}

// calcular a posicao de um diretorio dentro do disco
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

// procurar diretorio
long int find_dir(long int father_address, char *dir_name)
{
    
    // * Directory struct instance.
    Directory *directory_instance = returnDirPhysicalLocation(father_address);

    if (directory_instance->first_int == 0)
    {
        printf("\n\nDiretorio > %s < não encontrado...\n\n",dir_name);
        return 0;
    }

    // * Set the first child address
    long int dir_address = physicalAddress(xReadBlock.block_size, directory_instance->first_int);

    // * Begin the searching
    long int directory_address = 0;
    long int directory_block = directory_instance->first_int;
    while (directory_address == 0)
    {

        // * Read the directory
        lseek(xDisc, dir_address, SEEK_SET);
        read(xDisc, directory_instance, xReadBlock.block_size);
        
        if (strcmp(directory_instance->name, dir_name) == 0)
        {
            printf("\n\nDirectory found in block %ld.\n", directory_block);
            printDirectory(directory_instance);
            return directory_block;
        }
        if (directory_instance->next_int == 0)
        {
            //printf("Directory not found.\n");
            return 0;
        }
        dir_address = directory_instance->next_int * xReadBlock.block_size + 1;
        directory_block = directory_instance->next_int;
    }

    printf("\n\nDiretorio > %s < não encontrado...\n\n",dir_name);

};


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

    printf("\nDirectory Father ID: %d \n", inodeAddressFather);
    printf("\nDirectory Father ADDRESS: %d \n", father_address);
    struct directory *directory_instance = (struct directory *)malloc(xReadBlock.block_size);
    lseek(xDisc, father_address, SEEK_SET);
    read(xDisc, directory_instance, xReadBlock.block_size);

    long int child_address = directory_instance->first_int;
    printf("child_address: %ld\n", child_address);
    InodeNumberNameDir * inodeSet = (struct InodeNumberNameDir *) malloc(sizeof(struct InodeNumberNameDir));
     for (int i = 0; i < 64; ++i) {
        inodeSet->inodeNumbers[i] = 0; // You can initialize with any default value
    }
    
    int contador = 0;

    if (child_address != 0){
        struct directory *child_instance = (struct directory *)malloc(xReadBlock.block_size);
        rootBlocks[contador] = child_address;
        lseek(xDisc, child_address * xReadBlock.block_size + 1, SEEK_SET);
        read(xDisc, child_instance, xReadBlock.block_size);
        inodesNumbers[contador] = child_instance->inode;
        inodeNames[contador]=strdup(child_instance->name);
        while (child_instance->next_int != 0)
            {
                contador++;
                child_address = child_instance->next_int;
                rootBlocks[contador] = child_address;
                lseek(xDisc, child_instance->next_int * xReadBlock.block_size + 1, SEEK_SET);
                read(xDisc, child_instance, xReadBlock.block_size);
                inodesNumbers[contador] = child_instance->inode;
                inodeNames[contador]=strdup(child_instance->name);
            }

        for (size_t i = 0; i < contador+1; i++)
        {

            printf("*Nome: %s\n", inodeNames[i]);
            printf("*inodesNumbers: %d\n", inodesNumbers[i]);
            inodeSet->inodeNumbers[i] = inodesNumbers[i];
            inodeSet->rootBlocks[i] = rootBlocks[i];
            snprintf(inodeSet->dirNames[i], sizeof(inodeNames[i]), inodeNames[i], i);
            printf("*inodeSet: %s\n", inodeSet->dirNames[i]);
        }

    }

    return inodeSet;

}


/**
* \authors Sandy Hoffmann and Leonardo de Souza Fiamoncini.
* \since 24/04/2023.
* \brief Function that shows all directories in memory, based on the folder name and the father's address

**/
void show_all_directories()
{

    long int father_address = xReadBlock.inode_directory_start;
    struct directory *directory_instance = (struct directory *)malloc(xReadBlock.block_size);
    lseek(xDisc, father_address, SEEK_SET);
    read(xDisc, directory_instance, xReadBlock.block_size);
    printf("\n\nDirectory Father: %s \n", directory_instance->name);

    long int child_address = directory_instance->first_int;
    struct directory *child_instance = (struct directory *)malloc(xReadBlock.block_size);
    lseek(xDisc, child_address * xReadBlock.block_size + 1, SEEK_SET);
    read(xDisc, child_instance, xReadBlock.block_size);

    print_nexts( child_instance, child_address, 1);

};

/**
* \authors Sandy Hoffmann and Leonardo de Souza Fiamoncini.
* \since 01/05/2023.
* \brief Print all directories in memory, based on the folder name and the father's address

**/
void print_nexts( struct directory *child_instance, long int child_address, int level){
        
    for (int i = 0; i < level; i++)
    {
        printf("--");
    }

    printf("Directory: %s (Inode - %d) \n", child_instance->name, child_instance->inode);

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

/**
 * \authors Sandy Hoffmann and Leonardo de Souza Fiamoncini.
 * \brief Function that allocates data in memory using the directs and indirects pointers of the inode.
 * \date 05/06/2023
 * 
 */

long int * allocate_data(char *data){
    // ! First desconsidering the directs pointers
    // ! Indirect 1� is working, 2� is working, 3� in progress

    long int *indirectArray = malloc(4 * sizeof(long int));
    
    // * Information about the pointers
    // * each indirect pointer to a block of data (block_size or 4096 bytes)
    long int total_data_indirects_1 = xReadBlock.block_size / sizeof(long int);
    long int total_data_indirects_2 = pow(xReadBlock.block_size / sizeof(long int),2);
    long int total_data_indirects_3 = pow(xReadBlock.block_size / sizeof(long int),3);

    // * Alocando todos os blocos necess�rios de acordo com os indirects necess�rios

    // ? Considering a dump file data
    int text_file = open(data, O_RDONLY);

    char *buffer_data = (char *)malloc(xReadBlock.block_size);

    // * Parameter to see if there is a need to change the indirect pointer
    long int total_blocks_read = 0;

    // ? Indirect pointers counter
    long int * all_indirect_counter = malloc(3 * sizeof(long int));
    memset(all_indirect_counter, 0, 3 * sizeof(long int));
    // * First indirect long int pointer

    // ? Indirect pointers
    long int * all_active_indirect = malloc(3 * sizeof(long int));
    memset(all_active_indirect, 0, 3 * sizeof(long int));

    int reading_data = read(text_file, buffer_data, xReadBlock.block_size);

    while (reading_data > 0)
    {
        // * First Indirect
        long int block_address_allocate = return_free_data_bit(xDisc, xReadBlock);
        // * ifs control the indirect pointer
        if (total_blocks_read == 0){
            all_active_indirect[0] = block_address_allocate;
            indirectArray[0] = block_address_allocate;
            block_address_allocate = return_free_data_bit(xDisc, xReadBlock);
        } else if(total_blocks_read == total_data_indirects_1){
            all_active_indirect[0] = block_address_allocate;
            indirectArray[1] = block_address_allocate;
            block_address_allocate = return_free_data_bit(xDisc, xReadBlock);
            all_indirect_counter[0] = 0;
        } else if (total_blocks_read == total_data_indirects_2){
            all_active_indirect[0] = block_address_allocate;
            indirectArray[2] = block_address_allocate;
            block_address_allocate = return_free_data_bit(xDisc, xReadBlock);
            all_indirect_counter[0] = 0;
        } else if (total_blocks_read == total_data_indirects_3){
            printf("Error: File too big");
            break;
        } 

        if (total_blocks_read < total_data_indirects_1){
            write_indirect(buffer_data, block_address_allocate, all_indirect_counter[0], all_active_indirect[0],1);
            all_indirect_counter[0]++;
        } 
        else if (total_blocks_read < total_data_indirects_2){
            if (total_blocks_read % total_data_indirects_1 == 0){
                all_active_indirect[1] = block_address_allocate;
                write_indirect(buffer_data, block_address_allocate, all_indirect_counter[0], all_active_indirect[0],2);
                block_address_allocate = return_free_data_bit(xDisc, xReadBlock);
                all_indirect_counter[0]++;
                all_indirect_counter[1] = 0;
            }
            printf("new_block_address_allocate: %ld\n", block_address_allocate);
            write_indirect(buffer_data, block_address_allocate, all_indirect_counter[1], all_active_indirect[1],1);
            all_indirect_counter[1]++;
        } 
        else if (total_blocks_read < total_data_indirects_3){
            if (total_blocks_read % total_data_indirects_2 == 0){
                all_active_indirect[2] = block_address_allocate;
                write_indirect(buffer_data, block_address_allocate, all_indirect_counter[0], all_active_indirect[0],2);
                block_address_allocate = return_free_data_bit(xDisc, xReadBlock);
                all_indirect_counter[0]++;
                all_indirect_counter[1] = 0;
                all_indirect_counter[2] = 0;
            }
            if (total_blocks_read % total_data_indirects_1 == 0){
                all_active_indirect[1] = block_address_allocate;
                write_indirect(buffer_data, block_address_allocate, all_indirect_counter[1], all_active_indirect[1],2);
                block_address_allocate = return_free_data_bit(xDisc, xReadBlock);
                all_indirect_counter[1]++;
                all_indirect_counter[2] = 0;
            }
            write_indirect(buffer_data, block_address_allocate, all_indirect_counter[2], all_active_indirect[2],1);
            all_indirect_counter[2]++;
        
        }
        
        memset(buffer_data, 0, xReadBlock.block_size);

        reading_data = read(text_file, buffer_data, xReadBlock.block_size);

        total_blocks_read++;
        
    }

    // * Last of the array is the size of the file in blocks
    indirectArray[3] = total_blocks_read;
    return indirectArray;
}

/**
 * \authors Sandy Hoffmann and Leonardo de Souza Fiamoncini.
 * \brief Function that allocates data in first indirect inode.
 * \date 18/06/2023
 */
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

// ler dados de um inode
void read_data(struct Inode *inode){
        
    // * Information about the pointers
    // * each indirect pointer to a block of data (block_size or 4096 bytes)
    long int total_data_indirects_1 = xReadBlock.block_size / sizeof(long int);
    long int total_data_indirects_2 = pow(xReadBlock.block_size / sizeof(long int),2);
    long int total_data_indirects_3 = pow(xReadBlock.block_size / sizeof(long int),3);

    long int total_size_block = inode->file_size;
    long int total_blocks_read = 0;

    long int active_indirect = 0;

    long int * all_indirects = malloc(sizeof(long int)*3);
    memset(all_indirects, 0, sizeof(long int)*3);

    while (total_blocks_read < total_size_block){
        if (all_indirects[0] == 0){
            all_indirects[0] = inode->indirect1;
            for (int i = 0; i < total_data_indirects_1; i++){
                read_block(all_indirects[0], i);

                total_blocks_read++;
                if (total_blocks_read == total_size_block){
                    printf("\nTotal blocks read 1: %ld \n", total_blocks_read);
                    break;
                }
            }
        } else if (total_blocks_read == total_data_indirects_1){
            all_indirects[0] = inode->indirect2;
            for (int i = 0; i < total_data_indirects_2; i++){
                long int physical_offset = physicalAddress(xReadBlock.block_size,all_indirects[0])+i*sizeof(long int);
                lseek(xDisc, physical_offset , SEEK_SET);
                read(xDisc, &all_indirects[1], sizeof(long int));
                for (int j = 0; j < total_data_indirects_1; j++){
                    read_block(all_indirects[1], j);
                    total_blocks_read++;
                    if (total_blocks_read == total_size_block){
                        printf("\nTotal blocks read 2: %ld \n", total_blocks_read);
                        break;
                    }
                }
                if (total_blocks_read == total_size_block){
                    printf("\nTotal blocks read 3: %ld \n", total_blocks_read);
                    break;
                }
            }
            break;
        } else if (total_size_block == total_data_indirects_2){
            all_indirects[2] = inode->indirect3;
        } else {
            printf("Error: File too big");
            break;
        }
    }

}

//ler bloco
void read_block(long int block_address, long int i){
    long int numero = (long int)malloc(sizeof(long int));
    long int physical_offset_num = physicalAddress(xReadBlock.block_size,block_address)+i*sizeof(long int);
    lseek(xDisc, physical_offset_num , SEEK_SET);
    read(xDisc, &numero, sizeof(long int));

    char *buffer_data = (char *)malloc(xReadBlock.block_size);
    long int physical_offset = physicalAddress(xReadBlock.block_size, numero);
    lseek(xDisc, physical_offset, SEEK_SET);
    read(xDisc, buffer_data, xReadBlock.block_size);
    printf("%s", buffer_data);
}

// testar diretorios
void create_dump_directory_tree(){

    xmkdir("/pasta1","inputTest.txt", 0);
    xmkdir("/pasta1/pasta1.1","", 0);
    xmkdir("/pasta1/pasta1.2","", 0);
    xmkdir("/pasta1/pasta1.1/pasta1.1.1","", 0);
    xmkdir("/pasta2","inputTest2.txt", 0);
    xmkdir("/pasta2/pasta2.1","", 0);
    xmkdir("/pasta3","", 0);
    xmkdir("/pasta4","", 0);
    xmkdir("/pasta5","", 0);
    show_all_directories();

}