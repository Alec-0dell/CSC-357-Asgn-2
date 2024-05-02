#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>

#define MAX_NAME_LENGTH 32
#define MAX_INODES 1024


typedef struct {
    uint32_t inode_number;
    char type; // 'd' for directory, 'f' for file
} inode;

inode inodes[MAX_INODES];


int main(int argc, char *argv[])
{
    int inodes_list_file;
    int i = 0;

    // check for correct arguments
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file_system_directory>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Check if the specified directory exists
    if(chdir(argv[1]) != 0){
        fprintf(stderr, "Error: Directory '%s' not found.\n", argv[1]);
        return EXIT_FAILURE;
    }

    inodes_list_file = open("./inodes_list", O_RDWR);
    if (inodes_list_file == -1) {
        fprintf(stderr, "Error: Unable to open inodes_list file.\n");
        exit(EXIT_FAILURE);
    }

    for (int j = 0; j < MAX_INODES; j++)
    {
        read(inodes_list_file, &inodes[j], 5);
    }


    for(i = 0; i < MAX_INODES; i++){
        if(inodes[i].inode_number < UINT_MAX && (inodes[i].type == 'f' || inodes[i].type == 'd')){
            printf("inode #: %u file type: %c \n", inodes[i].inode_number, inodes[i].type);
        }
    }

    close(inodes_list_file);
    
    return EXIT_SUCCESS;
}
