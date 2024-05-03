#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include <stdint.h>

#define MAX_NAME_LENGTH 32
#define MAX_INODES 1024

typedef struct
{
    uint32_t inode_number;
    char type; // 'd' for directory, 'f' for file
} inode;

typedef struct
{
    uint32_t inode_number;
    char name[MAX_NAME_LENGTH];
} dir_ent;

int ls(int inodes_idx, inode *inodes, dir_ent *dir);
int cd(int inodes_idx, char *target, dir_ent *dir, int *cur_inode_idx);
int make_dir(int inodes_idx, char *dirname, dir_ent *dir, uint32_t *size);
int tch(int inodes_idx, char *filename, dir_ent *dir, uint32_t *size);
