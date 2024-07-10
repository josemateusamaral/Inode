#ifndef _super_h
#define _super_h
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

struct SuperBlock
{
    long int magic;
    int block_size;
    int inode_size;
    int inode_per_block;
    int reserved_blocks;

    int bmap_inode_start;
    int bmap_data_start;
    int inode_start;
    int inode_directory_start;
    int inode_total;
    int block_data_total;
    int data_start;
};

struct SuperBlock data_calculation_hd(int size_block, int size_inode, int size_hd_gb);
struct SuperBlock create_super_block(int size_block, int size_inode, int size_hd_gb);
void printSuperBlock(int xDisc);
struct SuperBlock read_superblock(int xDisc);
#endif