
#include "pathname.h"
#include "directory.h"
#include "inode.h"
#include "diskimg.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

/**
 * TODO
 */
int pathname_lookup(struct unixfilesystem *fs, const char *pathname) {

    if (pathname == NULL || fs == NULL) {
        return -1; 
    }
    if (strlen(pathname) == 0) {
        return -1; 
    }
    char *path = strdup(pathname);
    if (path == NULL) {
        return -1; 
    }
    char *token = strtok(path, "/");
    int inumber = ROOT_INUMBER; 
    struct inode in;
    while (token != NULL) {

        if (inode_iget(fs, inumber, &in) == -1) {
            free(path);
            return -1; 
        }

        if ((in.i_mode & IALLOC) == 0) {
            free(path);
            return -1;
        }

        int found = 0;
        for (int i = 0; i < in.i_size0 / sizeof(struct direntv6); i++) {
            struct direntv6 entry;

            if (directory_getentry(fs, inumber, i, &entry) == -1) {
                free(path);
                return -1; 
            }
        
            if (strcmp(entry.d_name, token) == 0) {
                inumber = entry.d_inumber; 
                found = 1;
                break;
            }
        }
        
        if (!found) {
            free(path);
            return -1;
        }

        token = strtok(NULL, "/");
    
    }
    free(path);
    return inumber;
}
