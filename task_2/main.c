#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

/* Print a filename with proper indentation */
static void print(const char *filename, int depth) {
    for (int i = 0; i < depth; i++) {
        printf("  "); 
    }
    printf("%s\n", filename); 
}

/* Construct a subdirectory path by concatenating a directory name to a base path */
static char *get_sub_path(const char *path, const char *dirname) {
    char *result = malloc(strlen(path) + strlen(dirname) + 2); // +2 bytes for slash and null terminator
    if (result == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }
    sprintf(result, "%s/%s", path, dirname);
    return result;
}

/* Recursive function to list files and sub-directories for a given path,
   keeping track of the depth for printing indentation */
static void ls(const char *path, int depth) {
    DIR *dirp;
    struct dirent *dp;

    if ((dirp = opendir(path)) == NULL) {
        perror("Couldn't open directory");
        return;
    }

    // Loop through each entry in the directory
    while ((dp = readdir(dirp)) != NULL) {
        const char *d_name = dp->d_name;
        if (d_name[0] == '.') {
            continue; // Skip hidden files
        }

        print(d_name, depth); 

        if (dp->d_type == DT_DIR) { // If it's a directory, recursively call ls
            char *sub_path = get_sub_path(path, d_name);
            ls(sub_path, depth + 1); // Increment depth for subdirectories
            free(sub_path); // Free memory allocated by get_sub_path()
        }
    }

    closedir(dirp);
}

int main() {
    // Get the current working directory
    //  Use the env variable instead of calling getcwd()
    //  This way we avoid dealing with buffers 
    char *cwd = getenv("PWD"); 

    printf("%s\n", cwd); // Print the base directory path
    ls(cwd, 1); // Start listing files and directories with a depth of 1
    
    return 0;
}
