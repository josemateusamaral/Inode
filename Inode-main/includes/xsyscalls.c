// disco montado
int xDisc;
int xSuperBlock;

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
void xFormat(int xDisc)
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

    long int total_data = block.data_start;                     // * bytes 0 - 16527862 byte
    int occupied_blocks = ROUND(total_data / block.block_size); // * blocks = 4036 occupied blocks


    for (int i = 0; i < occupied_blocks; i++)
    {
        return_free_data_bit(xDisc, block);
    }

    printf("Occupied blocks: %d\n", occupied_blocks);

    // for (int i = 0; i < total_blocks_full_byte + 1; i++){
    //     printf("Byte 2: %x\n", list_data_byte[i]);
    // }

    // list_data_bits[total_data_bits - 1] = resto;

    // * 11111111110000000000000000000000000000000000
    // * 1 byte = 8 bits
    // * 1 byte completo = 11111111
    // write(xDisc, list_data_bits, total_data_bits);
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

    create_dump_directory_tree(xDisc, ReadBlock);
}

// montar disco
void xmount()
{
    if ((xDisc = open("bla.hd", O_RDWR)) == -1)
    {
        printf("Erro ao montar o disco...");
        exit(1);
    }
}

// desmontar disco
void xdismount()
{
    close(xDisc);
}