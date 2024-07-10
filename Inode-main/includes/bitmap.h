/**
 * \authors Sandy Hoffmann and Leonardo de Souza Fiamoncini.
 * \since 10/04/2023.
 **/

#ifndef _bitmap_h
#define _bitmap_h
#include "super.h"

int return_free_inode_bit(int fdHd, struct SuperBlock ReadBlock);
int return_free_data_bit(int fdHd, struct SuperBlock ReadBlock);
int return_free_bit(int fdHd, long int total, long int bmap_start);
#endif