
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
    if (pathname == NULL || fs == NULL || strlen(pathname) == 0) {
        return -1; // Input inválido
    }

    // Copia del pathname para usar con strtok
    char *path = strdup(pathname);
    if (path == NULL) {
        return -1; // Error de memoria
    }

    int inumber = ROOT_INUMBER;
    struct direntv6 entry;
    char *token = strtok(path, "/");

    while (token != NULL) {
        // Buscar el token (componente del path) en el directorio actual
        if (directory_findname(fs, token, inumber, &entry) == -1) {
            free(path);
            return -1; // No se encontró el componente
        }

        // Actualizar el inumber al nuevo directorio/archivo encontrado
        inumber = entry.d_inumber;

        // Siguiente componente
        token = strtok(NULL, "/");
    }

    free(path);
    return inumber; // Éxito: devuelve el inumber del último componente del path
}
