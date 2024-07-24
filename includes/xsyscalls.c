#include "xsyscalls.h"

// formatar disco rapidamente sem limpar os blocos
void xformatFast()
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
void xformat()
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

    Directory * root = create_dir("/", bit_free_inode);
    struct Inode *inode = create_inode("",DIRECTORY, NULL);

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
    xformatFast();
    sleep(2);
    xformat(xDisc);
    sleep(2);

    struct SuperBlock ReadBlock;
    char *buffer = malloc(4096);
    lseek(xDisc, 0, SEEK_SET);
    read(xDisc, buffer, 4096);
    memcpy(&ReadBlock, buffer, sizeof(struct SuperBlock));

    //create_dump_directory_tree();
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
void xmkdir(char * nomePasta){
    CreateEntry(nomePasta,DIRECTORY);
}

// criar arquivo
void xopen(char * nomeArquivo,char * tipo){


    //Directory * entrada = find_dir(xpath,nomeArquivo);
    //printf("ENTRADA\n");
    //printDirectory(entrada);
    //XFILE arquivo;
    CreateEntry(nomeArquivo,REGULAR_FILE);
    /*
    if (strlen(file_name) > 1){
        indirects = allocate_data(file_name);
    }
    printf("\narquivo: %s - Inode: %ld",indirec)
    */

}

