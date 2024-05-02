#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>



int main(int argc, char *argv[])
{
    DIR *dp;
    struct dirent *de = NULL;
    struct stat fstat;

    // check for correct arguments
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file_system_directory>\n", argv[0]);
        return EXIT_FAILURE;
    }

    dp = opendir(argv[1]);

    // Check if the specified directory exists
    if (dp == NULL) {
        fprintf(stderr, "Error: Directory '%s' not found.\n", argv[1]);
        return EXIT_FAILURE;
    }
    
    while ((de = readdir(dp)) != NULL) {
        stat(de->d_name, &fstat);
        printf("inode: %llu, st_size: %lld, name: %s\n", de->d_ino, fstat.st_size, de->d_name);
    }

    closedir(dp);
    
    return EXIT_SUCCESS;
}
