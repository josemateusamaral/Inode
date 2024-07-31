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
Directory * returnDirPhysicalLocation(long int block);
Directory * find_dir(long int father_address, char *dir_name);
void xls();
void print_nexts(struct directory *child_instance, long int child_address, int level);
void CreateEntry( char * dir_name, int type );
void allocate_data(char * data,int tamanho,struct Inode * arquivo, long int posicaoEscrita);
void write_block(char *data, long int block_address);
char * read_block(long int block_address, long int i);
InodeNumberNameDir * return_child_inodes(int inodeAddressFather, InodeNumberNameDir * sh_mem);
void write_indirect(char *data, long int block_address, long int indirect_counter, long int active_indirect, int level);

#endif