
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
    //Implement code here
    if (pathname == NULL || fs == NULL) {
        return -1; // Invalid input
    }
    if (strlen(pathname) == 0) {
        return -1; // Empty pathname
    }
    char *path = strdup(pathname);
    if (path == NULL) {
        return -1; // Memory allocation error
    }
    char *token = strtok(path, "/");
    int inumber = ROOT_INUMBER; // Start with the root inode number
    struct inode in;
    while (token != NULL) {
        if (inode_iget(fs, inumber, &in) == -1) {
            free(path);
            return -1; // Error getting inode
        }
        if ((in.i_mode & IALLOC) == 0) {
            free(path);
            return -1; // Inode not allocated
        }
        int found = 0;
        for (int i = 0; i < in.i_size0 / sizeof(struct direntv6); i++) {
            struct direntv6 entry;
            
            // Obtener la i-ésima entrada del directorio
            if (directory_getentry(fs, inumber, i, &entry) == -1) {
                free(path);
                return -1; // Error al obtener la entrada del directorio
            }
        
            // Comparar el nombre de la entrada con el token actual del path
            if (strcmp(entry.d_name, token) == 0) {
                inumber = entry.d_inumber; // Actualizar el inumber si se encuentra
                found = 1;
                break;
            }
        }
        
        // Si no se encontró el componente del path, error
        if (!found) {
            free(path);
            return -1;
        }
        
        // Avanzar al siguiente componente del path
        token = strtok(NULL, "/");
    
    }
    free(path);
    return inumber;
}