#include <stdint.h>
#include "fs_simulator.h"

inode inodes[MAX_INODES];
uint32_t size = 0;
int cur_inode_idx;
dir_ent dir[MAX_INODES];

int main(int argc, char *argv[])
{
    int inodes_list_file;
    char com_line[38]; // max 5 char command + ' ' + max 32 char filename
    int com_ln_idx = 0;
    char command[6]; // for our case the most number of arguments is two.
    char name[33];

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

    //open inodes file to create inodes array
    inodes_list_file = open("./inodes_list", O_RDWR);
    if (inodes_list_file == -1)
    {
        fprintf(stderr, "Error: Unable to open inodes_list file.\n");
        exit(EXIT_FAILURE);
    }

    //creates inodes array and initializes size
    for (int j = 0; j < MAX_INODES; j++)
    {
        read(inodes_list_file, &inodes[size], 5);
        if (inodes[size].inode_number < UINT_MAX && (inodes[size].type == 'f' || inodes[size].type == 'd'))
        {
            size++; // valid i-node
        }
    }

    //boot command line
    while (1)
    {
        // clear command and name arrays
        command[0] = 0;
        for (int i = 0; i < 32; i++)
        {
            name[i] = 0;
        }
        printf("> ");
        //get next line
        if (fgets(com_line, sizeof(com_line), stdin) != NULL)
        {
            //split the command and arguments
            for (com_ln_idx = 0; com_line[com_ln_idx] != ' ' && com_line[com_ln_idx] != '\n'; com_ln_idx++)
            {
                command[com_ln_idx] = com_line[com_ln_idx];
            }
            command[com_ln_idx] = 0;
            com_ln_idx++;
            for (int arg_idx = com_ln_idx; com_line[com_ln_idx] != '\n' && (com_ln_idx - arg_idx) < 32; com_ln_idx++)
            {
                name[com_ln_idx - arg_idx] = com_line[com_ln_idx];
                if (com_line[com_ln_idx + 1] == '\n')
                {
                    name[com_ln_idx - arg_idx + 1] = 0;
                }
            }
            command[++com_ln_idx] = 0;
            //run the correct command 
            if (strcmp(command, "ls") == 0) // ls command
            {
                ls();
            }
            else if (strcmp(command, "cd") == 0) // cd command
            {
                cd(name);
            }
            else if (strcmp(command, "mkdir") == 0) // mkdir command
            {
                make_dir(name);
            }
            else if (strcmp(command, "touch") == 0) // touch command
            {
                tch(name);
            }
            else if (strcmp(command, "exit") == 0) // exit command
            {
                return 0;
            }
            else // unknown command command
            {
                printf("No such command \n");
            }
        }
    }

    close(inodes_list_file);

    return EXIT_SUCCESS;
}

int ls(void)
{
    int m = 0;
    int dir_ls;
    char file_path[32];
    snprintf(file_path, 32, "./%d", inodes[cur_inode_idx].inode_number);
    dir_ls = open(file_path, O_RDWR);

    if (inodes[cur_inode_idx].type == 'f')
    {
        return EXIT_SUCCESS;
    }

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

int cd( char *target)
{
    int m = 0;
    int dir_ls;
    char dir_path[32];
    snprintf(dir_path, 32, "./%d", inodes[cur_inode_idx].inode_number);
    dir_ls = open(dir_path, O_RDWR);
    if (dir_ls == -1)
    {
        fprintf(stderr, "Error: Unable to open current directory.\n");
        exit(EXIT_FAILURE);
    }

    while (read(dir_ls, &dir[m], 36) > 0)
    {
        if (strcmp(dir[m].name, target) == 0)
        {
            for (int i = 0; i < size; i++)
            {
                if (dir[m].inode_number == inodes[i].inode_number)
                {
                    cur_inode_idx = i;
                    close(dir_ls);
                    return EXIT_SUCCESS;
                }
            }
        }
        m++;
    }

    close(dir_ls);
    return EXIT_FAILURE;
}

int make_dir( char *dirname)
{
    int m = 0;
    int new_file;
    int dirr;
    char file_mode = 'd';
    char dir_path[32];
    char file_path[32];
    char parent[33];
    FILE *dirrr;
    snprintf(dir_path, 32, "./%d", inodes[cur_inode_idx].inode_number);
    dirr = open(dir_path, O_RDWR);
    if (dirr == -1)
    {
        fprintf(stderr, "Error: Unable to open current directory.\n");
        exit(EXIT_FAILURE);
    }

    // Check if file already exists
    while (read(dirr, &dir[m], 36) > 0)
    {
        if (strcmp(dir[m].name, dirname) == 0)
        {
            return EXIT_SUCCESS;
        }
        m++;
    }

    // create new file
    snprintf(file_path, 32, "./%d", size);
    new_file = open(file_path, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (new_file == -1)
    {
        fprintf(stderr, "Error: Unable to create file '%s'.\n", dirname);
        return EXIT_FAILURE;
    }
    close(new_file);

    // set up directory
    // its own number: .
    // its parents number: ..
    dirrr = fopen(file_path, "a");
    parent[0] = '.';
    parent[1] = '.';
    for (int i = 2; i < 33; i++)
    {
        parent[i] = 0;
    }
    fwrite(&size, sizeof(uint32_t), 1, dirrr);
    fwrite(&parent[1], sizeof(char), 32, dirrr);
    fwrite(&cur_inode_idx, sizeof(uint32_t), 1, dirrr);
    fwrite(parent, sizeof(char), 32, dirrr);
    fclose(dirrr);

    // update directory
    dirrr = fopen(dir_path, "a");
    fwrite(&size, sizeof(uint32_t), 1, dirrr);
    fwrite(dirname, sizeof(char), 32, dirrr);
    fclose(dirrr);

    // update inodes
    dirrr = fopen("./inodes_list", "a");
    fwrite(&size, sizeof(uint32_t), 1, dirrr);
    fwrite(&file_mode, sizeof(char), 1, dirrr);
    fclose(dirrr);

    size++;

    return EXIT_SUCCESS;
}

int tch( char *filename)
{
    int m = 0;
    int new_file;
    int dirr;
    char file_mode = 'f';
    char dir_path[32];
    char file_path[32];
    FILE *dirrr;
    snprintf(dir_path, 32, "./%d", inodes[cur_inode_idx].inode_number);
    dirr = open(dir_path, O_RDWR);
    if (dirr == -1)
    {
        fprintf(stderr, "Error: Unable to open current directory.\n");
        exit(EXIT_FAILURE);
    }

    // Check if file already exists
    while (read(dirr, &dir[m], 36) > 0)
    {
        if (strcmp(dir[m].name, filename) == 0)
        {
            return EXIT_SUCCESS;
        }
        m++;
    }

    // create new file
    snprintf(file_path, 32, "./%d", size);
    new_file = open(file_path, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (new_file == -1)
    {
        fprintf(stderr, "Error: Unable to create file '%s'.\n", filename);
        return EXIT_FAILURE;
    }
    write(new_file, filename, 32); // fill the file with the file name

    close(new_file);

    // update directory
    dirrr = fopen(dir_path, "a");
    fwrite(&size, sizeof(uint32_t), 1, dirrr);
    fwrite(filename, sizeof(char), 32, dirrr);
    fclose(dirrr);

    // update inodes
    dirrr = fopen("./inodes_list", "a");
    fwrite(&size, sizeof(uint32_t), 1, dirrr);
    fwrite(&file_mode, sizeof(char), 1, dirrr);
    fclose(dirrr);

    size++;

    return EXIT_SUCCESS;
}
