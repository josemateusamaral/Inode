#include "inode.h"
#include "file_operations.h"

// criar inode
struct Inode *create_inode(char *content, int file_type, long int *indirects)
{
    struct Inode *inode = (struct Inode *)malloc(sizeof(struct Inode));
    inode->file_type = file_type;
    inode->creation_time = time(NULL);
    inode->modify_time = time(NULL);
    if (indirects != NULL)
    {
        //printf("Indirects 1 create inode: %ld\n", indirects[0]);
        inode->indirect1 = indirects[0];
        inode->indirect2 = indirects[1];
        inode->indirect3 = indirects[2];
        inode->file_size = indirects[3];
    }
    else
    {
        inode->indirect1 = 0;
        inode->indirect2 = 0;
        inode->indirect3 = 0;
        inode->file_size = 0;
    }

    //printf("Inode size: %ld\n", sizeof(struct Inode));

    if (strlen(content) == 0)
    {
        return inode;
    }

    return inode;
}

//alocar inode no disco
long int allocate_inode(int xDisc, struct SuperBlock ReadBlock, long int * indirects, int type){

    long int free_inode = return_free_inode_bit(xDisc, ReadBlock);
    struct Inode *inode_instance = (struct Inode *)malloc(ReadBlock.inode_size);
    inode_instance = create_inode("",type, indirects);
    long int physical_inode_address = ReadBlock.inode_start + free_inode * ReadBlock.inode_size;
    lseek(xDisc, physical_inode_address, SEEK_SET);
    write(xDisc, inode_instance, ReadBlock.inode_size);
    

    return free_inode;
}

// printar inode
void printInode_INT(long int inode_address){
    long int physical_inode_address = xReadBlock.inode_start + inode_address * xReadBlock.inode_size;
    lseek(xDisc, physical_inode_address, SEEK_SET);
    struct Inode *inode = (struct Inode *)malloc(xReadBlock.inode_size);
    read(xDisc, inode, xReadBlock.inode_size);
    printf("inode->file_type: %d\n", inode->file_type);
    printf("inode->file_size: %d\n", inode->file_size);
    printf("inode->creation_time: %ld\n", inode->creation_time);
    printf("inode->modify_time: %ld\n", inode->modify_time);
    printf("inode->indirect1: %ld\n", inode->indirect1);
    printf("inode->indirect2: %ld\n", inode->indirect2);
    printf("inode->indirect3: %ld\n", inode->indirect3);
}

// printar inode
void printInode_OBJ( struct Inode * inode){
    printf("inode->file_type: %d\n", inode->file_type);
    printf("inode->file_size: %d\n", inode->file_size);
    printf("inode->creation_time: %ld\n", inode->creation_time);
    printf("inode->modify_time: %ld\n", inode->modify_time);
    printf("inode->indirect1: %ld\n", inode->indirect1);
    printf("inode->indirect2: %ld\n", inode->indirect2);
    printf("inode->indirect3: %ld\n", inode->indirect3);
}

// ler dados de um inode
struct Inode * readInode(long int inode_address){
    long int physical_inode_address = xReadBlock.inode_start + inode_address * xReadBlock.inode_size;
    lseek(xDisc, physical_inode_address, SEEK_SET);
    struct Inode *inode = (struct Inode *)malloc(xReadBlock.inode_size);
    read(xDisc, inode, xReadBlock.inode_size);
    return inode;
}
