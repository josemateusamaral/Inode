/**
 * \authors Sandy Hoffmann and Leonardo de Souza Fiamoncini.
 * \since 24/04/2023.
 **/

#ifndef _file_operations_h
#define _file_operations_h
#include "file_operations.h"

typedef struct InodeNumberNameDir 
{
    long int inodeNumbers[64];
    long int rootBlocks[64];
    char dirNames[64][255];

} InodeNumberNameDir;


long int physicalAddress(int blockSize, long int block);
Directory *returnDirPhysicalLocation(int xDisc, struct SuperBlock ReadBlock, long int block);
void xmkdir( struct SuperBlock ReadBlock, char *dir_name, char *file_name, long int father_address);
long int find_dir(int xDisc, struct SuperBlock ReadBlock, long int father_address, char *dir_name);
void show_all_directories(int xDisc, struct SuperBlock ReadBlock);
void print_nexts(int xDisc, struct SuperBlock ReadBlock, struct directory *child_instance, long int child_address, int level);
void create_dump_directory_tree(int xDisc, struct SuperBlock ReadBlock);
long int * allocate_data(int xDisc, struct SuperBlock ReadBlock, char *data);
void write_block(int xDisc, struct SuperBlock ReadBlock, char *data, long int block_address);
void read_data(int xDisc, struct SuperBlock ReadBlock, struct Inode *inode);
void read_block(int xDisc, struct SuperBlock ReadBlock, long int block_address, long int i);
InodeNumberNameDir * return_child_inodes(int inodeAddressFather,struct SuperBlock ReadBlock,int xDisc, InodeNumberNameDir * sh_mem);
void write_indirect(int xDisc,struct SuperBlock ReadBlock,char *data, long int block_address, long int indirect_counter, long int active_indirect, int level);
#endif