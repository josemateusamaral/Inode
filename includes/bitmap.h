/**
 * \authors Sandy Hoffmann and Leonardo de Souza Fiamoncini.
 * \since 10/04/2023.
 **/

#ifndef _bitmap_h
#define _bitmap_h

#include "super.h"
#include "xsyscalls.h"

int return_free_inode_bit();
int return_free_data_bit();
unsigned char return_bit_moved(unsigned char bitset, int i);
int return_free_bit(int fdHd, long int total, long int bmap_start);

#endif