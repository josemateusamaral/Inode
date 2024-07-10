#ifndef _directory_h
#define _directory_h

typedef struct directory
{
	char name[60];
	int inode;
	unsigned long int first_int;
	struct directory *first_structure;
	unsigned long int next_int;
	struct directory *next_structure;
} Directory;

#endif
