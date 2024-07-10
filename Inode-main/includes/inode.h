#ifndef _inode_h
#define _inode_h

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/**
 * \authors Sandy Hoffmann and Leonardo de Souza Fiamoncini.
 * \since 25/03/2023.
 * \brief This enumeration defines different types of files that can be stored in the file system. Each type is assigned an integer value, starting from 1.
 **/
enum FileType
{
    REGULAR_FILE = 1,
    DIRECTORY = 2,
    SYMBOLIC_LINK = 3,
    BLOCK_DEVICE = 4,
    CHARACTER_DEVICE = 5,
    PIPE = 6
};

/**
 * \authors Sandy Hoffmann and Leonardo de Souza Fiamoncini.
 * \since 25/03/2023.
 * \update 05/06/2023. 
 * \brief This structure defines the inode itself, which is a data structure used in some file systems to represent a file or directory. It contains a pointer to a "HeadInode" structure (more on that later), an array of 6 integers representing the addresses of direct data blocks, and 3 pointers to "Indirect" structures, which are used to address additional data blocks in the case where the direct data blocks are not sufficient.
 * \update Struct with pointers to others structures cannot be find in the file system.
 * \ - Head elements will become attributes of the inode.
 * \ - Indirect elements will become addresses of data blocks.
 **/

// ? indirects content can be stored at same data block (to save space)
// * If we are about to store the inode in disk, use the int to the data address
// * else, use the structure (to manipulate the data)

// 1 - read from hd - inode {..., indirect1: Indirect{7, Indirect{8, Indirect{9, NULL}}}
// 2 - write in hd - inode {..., indirect1: 7, indirect2: 8, indirect3: 9}
struct Inode
{
    int file_size;
    enum FileType file_type;
    time_t creation_time;
    time_t modify_time;
    long int direct_data[6];
    long int size;
    long int indirect1;
    long int indirect2;
    long int indirect3;
};

/**
 * \authors Sandy Hoffmann and Leonardo de Souza Fiamoncini.
 * \since 25/03/2023.
 * \brief This structure defines a "HeadInode", which contains metadata about a file or directory. It includes the file size, file type (as defined by the enumeration above), and creation and modification times (stored as time_t variables).
 **/
struct HeadInode
{
    int file_size;
    enum FileType file_type;
    time_t creation_time;
    time_t modify_time;
};

/**
 * \authors Sandy Hoffmann and Leonardo de Souza Fiamoncini.
 * \since 25/03/2023.
 * \brief This structure defines an "Indirect" structure, which is used to indirectly address data blocks when the direct data blocks are not sufficient. It includes a union of two data types: an unsigned long integer representing the address of a data block, or a pointer to another "Indirect" structure (used, for example, when a third level of indirection is needed). It also includes a pointer to the next "Indirect" structure in the linked list.
 * \update 05/06/2023. Now this estructure is used as a search mechanism for the data blocks.
 **/
struct Indirect
{
    union data
    {
        unsigned long int node;
        struct Indirect *indirect;
    } data;
    struct Indirect *next;
};

struct Inode *create_inode(char *content, int file_type, long int *indirects);
long int allocate_inode(int xDisc, struct SuperBlock ReadBlock, long int * indirects);
void printInode(int xDisc, struct SuperBlock ReadBlock, long int inode_address);
void readInode(int xDisc, struct SuperBlock ReadBlock,long int inode_number);
#endif