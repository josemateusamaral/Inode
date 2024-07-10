#include "directory.h"

// criar diretorio
Directory *create_dir(char *_name, int _inode)
{
    Directory *dir = (Directory *)malloc(sizeof(Directory));
    strcpy(dir->name, _name);
    dir->inode = _inode;
    dir->first_int = 0;
    dir->next_int = 0;
    return dir;
}