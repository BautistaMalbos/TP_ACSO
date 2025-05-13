#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "file.h"
#include "inode.h"
#include "diskimg.h"

/**
 * TODO
 */
int file_getblock(struct unixfilesystem *fs, int inumber, int blockNum, void *buf) {
    
    struct inode inp;
    int result = inode_iget(fs, inumber, &inp);
    if (result == -1) {
        return -1; // Error al obtener el inodo
    }
    if ((inp.i_mode & IALLOC) == 0) {
        return -1; // Inodo no asignado
    }
    if (blockNum < 0 || blockNum >= 8) {
        return -1; // Número de bloque inválido
    }
    int sector = inp.i_addr[blockNum];
    if (sector == 0) {
        return -1; // Sector no asignado
    }
    int read_bytes = diskimg_readsector(fs->dfd, sector, buf);
    if (read_bytes == -1) {
        return -1; // Error al leer el sector
    }
    
    return 0;
}

