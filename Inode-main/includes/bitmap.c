#include "bitmap.h"

#include "super.h"
#define SETBIT(a, n) ((a << n)+ 1)
#define CLEARBIT(a, n) (a & (0xff ^ (1 << n)))

int return_free_inode_bit(int xDisc, struct SuperBlock ReadBlock)
{
    return return_free_bit(xDisc, ReadBlock.inode_total, ReadBlock.bmap_inode_start);
}

int return_free_data_bit(int xDisc, struct SuperBlock ReadBlock)
{
    return return_free_bit(xDisc, ReadBlock.block_data_total, ReadBlock.bmap_data_start);
};

unsigned char return_bit_moved(unsigned char bitset, int i){
    printf("\nBitset: %d\n", bitset);
    unsigned char bit = (bitset >> 7 - i) & 1;
    return bit || 0b10000000;
}

int return_free_bit(int xDisc, long int total, long int bmap_start)
{
    char *bufferBitmap = (char *)malloc(total / 8);

    lseek(xDisc, bmap_start, SEEK_SET);
    read(xDisc, bufferBitmap, total / 8);

    for (int i = 0; i < total / 8; i++)
    {
        char hex_string[3];
        sprintf(hex_string, "%02hhx", bufferBitmap[i]);
        int statusBitMap = strtol(hex_string, NULL, 16);

        if (statusBitMap < 255)
        {
            int allocated = SETBIT(statusBitMap,1);
            long int casas = __builtin_popcount(allocated);
            lseek(xDisc, bmap_start + i, SEEK_SET);
            write(xDisc, &allocated, 1);
            
            return (i * 8) + casas;
        }
    }
    printf("\nN�o h� bits livres\n");
    return -1;
};