#include "xsyscalls.h"

// formatar disco rapidamente sem limpar os blocos
void xFormatFast()
{
    char buffer[4096];
    bzero(buffer, 4096);
    lseek(xDisc, 0, SEEK_SET);
    int totalHd1gb = (1024 * 1024 * 1024) / 4096;
    for (int i = 0; i < totalHd1gb; i++)
    {
        write(xDisc, buffer, 4096);
    }
}

/**
 * \authors Sandy Hoffmann and Leonardo de Souza Fiamoncini.
 * \since 23/04/2023.
 * \brief Formating a disk in Inode system means:
 * 1. Creating a superblock.
 * 1.1 Superblock will contain all the information about the disk.
 * 1.2 Superblock will be the first block of the disk.
 * 2. Allocate the first directory entry (root directory) = /.
 * 2.1 Allocate the inode for the root directory.
 * 3. Allocating the inode bitmap including this new instance of root directory.
 * 4. Allocating the data bitmap including the superblock location, inode bitmap, data bitmap, the root directory, and inode reserved sector.
 * \param xDisc The file containing the hd itself.
 **/
void xFormat()
{
    // * Inode size increased to 128 bytes (22/06/2023)
    struct SuperBlock block = create_super_block(4096, 128, 1);
    void *buffer = malloc(block.block_size);
    memcpy(buffer, &block, sizeof(struct SuperBlock));
    lseek(xDisc, 0, SEEK_SET);
    write(xDisc, buffer, block.block_size);
    free(buffer);

    // * Inode bitmap already mounted on hd
    int bit_free_inode = return_free_inode_bit(xDisc, block);

    Directory *root = create_dir("/", bit_free_inode);
    struct Inode *inode = create_inode("", 1, NULL);

    // * Write Inode in HD
    long int adress_first_inode = block.inode_start;
    lseek(xDisc, adress_first_inode, SEEK_SET);
    write(xDisc, inode, block.inode_size);

    // * Write Directory in HD
    long int adress_dir = block.inode_directory_start;
    lseek(xDisc, adress_dir, SEEK_SET);
    write(xDisc, root, sizeof(Directory));

    // * Data bitmap represents the data blocks that are free or not.
    long int total_data = block.data_start;                    
    int occupied_blocks = ROUND(total_data / block.block_size);

    for (int i = 0; i < occupied_blocks; i++)
    {
        return_free_data_bit(xDisc, block);
    }

    printf("\n\nOccupied blocks: %d\n", occupied_blocks);

    xReadBlock = block;
}

// testar o discos
void xtest(int xDisc)
{
    xFormatFast();
    sleep(2);
    xFormat(xDisc);
    sleep(2);

    struct SuperBlock ReadBlock;
    char *buffer = malloc(4096);
    lseek(xDisc, 0, SEEK_SET);
    read(xDisc, buffer, 4096);
    memcpy(&ReadBlock, buffer, sizeof(struct SuperBlock));

    create_dump_directory_tree();
}

// montar disco
void xmount( char * pathDisco )
{
    if ((xDisc = open(pathDisco, O_RDWR)) == -1)
    {
        printf("Erro ao montar o disco...");
        exit(1);
    }
    xReadBlock = read_superblock(xDisc);

}

// desmontar disco
void xdismount()
{
    close(xDisc);
}

// criar diretorio
void xmkdir( char *dir_name, char *file_name, long int father_address){
    
    // * STEP 1 - Split the path into the directory name and the path itself.
    // * STEP 2 - Keep the reference to the father directory.
    // * STEP 3 - If father does not have first, then allocate the directory in the first.
    // *          Else, iterate into next's
    // * First it is necessary to split the path into the directory name and the path itself.

    char *dir_path = (char *)malloc(sizeof(char) * strlen(dir_name) + 1);
    strcpy(dir_path, dir_name);

    char *token = (char *)malloc(sizeof(char) * strlen(dir_name));
    token = strtok(dir_path, "/");

    char *nomeFormatado = (char *)malloc(sizeof(char) * strlen(dir_name));

    // * We have to iterate through the path, and make sure that this path exists and the folder does not.
    long int thisDirAddress = father_address;
    long int beforeAddress = 0;

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
                printf("Diretório já existe.\n");
                return;
            }

            if (strtok(NULL, "/") == NULL)
            {
                printf("\n\nParou no token: %s\n", token);
                break;
            }

            printf("Caminho não encontrado.\n");
        }

        strcpy(dir_path, token);
        token = strtok(NULL, "/");
        
    }
    printf("Endereco do before (pai): %ld\n", beforeAddress);
    printf("Endereco do elemento (this): %ld\n", thisDirAddress);

    // * If thisDirAddress is 0, then the directory does not exist
    // * If beforeAddress is 0, then the directory is the root directory

    if (thisDirAddress == 0){
        // * Creating the directory
        struct directory *directory_instance = (struct directory *)malloc(xReadBlock.block_size);
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
        
        // ? FIXED FILE TYPE, ALLOCATING STATIC TXT
        long int *indirects = (long int *)malloc(4 * sizeof(long int));
        if (strlen(file_name) > 1){
            indirects = allocate_data(file_name);
            
        }

        // * Allocating Inode

        long int inodeInt = allocate_inode(xDisc, xReadBlock, indirects);
        directory_instance->inode = inodeInt;

        // * Allocating the directory in the free data block
        long int physicalDirectoryAddress = physicalAddress(xReadBlock.block_size, free_data_block);
        lseek(xDisc, physicalDirectoryAddress, SEEK_SET);
        write(xDisc, directory_instance, xReadBlock.block_size);  

        printf("Diretorio criado com sucesso.\n");

    } else {
        printf("Diretorio já existe.\n");
    }


}