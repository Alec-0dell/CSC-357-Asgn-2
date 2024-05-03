#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>

#define MAX_NAME_LENGTH 32
#define MAX_INODES 1024

int ls(int);

typedef struct
{
    uint32_t inode_number;
    char type; // 'd' for directory, 'f' for file
} inode;

inode inodes[MAX_INODES];
int size = 0;

typedef struct
{
    uint32_t inode_number;
    char name[32];
} dir_ent;

dir_ent dir[MAX_INODES];

int main(int argc, char *argv[])
{
    int inodes_list_file;
    int cur_inode_idx = 0;
    char com_line[512];

    // check for correct arguments
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <file_system_directory>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Check if the specified directory exists
    if (chdir(argv[1]) != 0)
    {
        fprintf(stderr, "Error: Directory '%s' not found.\n", argv[1]);
        return EXIT_FAILURE;
    }

    inodes_list_file = open("./inodes_list", O_RDWR);
    if (inodes_list_file == -1)
    {
        fprintf(stderr, "Error: Unable to open inodes_list file.\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0, j = 0; j < MAX_INODES; j++)
    {
        read(inodes_list_file, &inodes[i], 5);
        if (inodes[i].inode_number < UINT_MAX && (inodes[i].type == 'f' || inodes[i].type == 'd'))
        {
            size++; //valid i-node
            i++;
        }
        else 
        {
            i--; //invalid i-node
        }
    }

    while (1)
    {
        printf("> ");
        if (fgets(com_line, sizeof(com_line), stdin) != NULL)
        {
            if (strcmp(com_line, "ls\n") == 0)
            {
                ls(cur_inode_idx);
            }
            else if (strcmp(com_line, "exit\n") == 0)
            {
                return 0;
            }
        }
    }

    close(inodes_list_file);

    return EXIT_SUCCESS;
}

int ls(int inodes_idx)
{
    int m = 0;
    int dir_ls;
    char file_path[32];
    snprintf(file_path, 32, "./%d", inodes_idx);
    dir_ls = open(file_path, O_RDWR);

    if (dir_ls == -1)
    {
        fprintf(stderr, "Error: Unable to open current directory.\n");
        exit(EXIT_FAILURE);
    }

    while (read(dir_ls, &dir[m], 36) > 0)
    {
        printf("%u %s \n", dir[m].inode_number, dir[m].name);
        m++;
    }

    return EXIT_SUCCESS;
}

int cd(int inodes_idx, char *file)
{
    int m = 0;
    int dir_ls;
    char file_path[32];
    snprintf(file_path, 32, "./%d", inodes_idx);
    dir_ls = open(file_path, O_RDWR);

    if (dir_ls == -1)
    {
        fprintf(stderr, "Error: Unable to open current directory.\n");
        exit(EXIT_FAILURE);
    }

    while (read(dir_ls, &dir[m], 36) > 0)
    {
        m++;
    }

    for (int n = 0; n < m; n++)
    {
        printf("%u %s \n", dir[n].inode_number, dir[n].name);
    }
    return EXIT_SUCCESS;
}
