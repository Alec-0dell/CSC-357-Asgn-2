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

int ls(int);
int cd(int, char *);
int make_dir(int, char *);
int tch(int, char *);
