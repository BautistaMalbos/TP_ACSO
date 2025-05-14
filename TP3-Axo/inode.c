#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "inode.h"
#include "diskimg.h"
#include "unixfilesystem.h"


/**
 * TODO
 */
int inode_iget(struct unixfilesystem *fs, int inumber, struct inode *inp) {
    if (inumber < 1) {
        return -1;
    }

    if (inumber > fs->superblock.s_isize) { //verifico que el inodo no sea mayor al tamaño del filesystem
        return -1;
    }

    int inode_per_sector = DISKIMG_SECTOR_SIZE / sizeof(struct inode); 
    int sector_num = INODE_START_SECTOR + (inumber - 1) / inode_per_sector; 
    int inode_index = (inumber - 1) % inode_per_sector;  

    struct inode buffer[ inode_per_sector ];  

    int read_bytes = diskimg_readsector(fs->dfd, sector_num, buffer); 
    if (read_bytes == -1) {
        return -1;
    }

    if (read_bytes != DISKIMG_SECTOR_SIZE) {
        return -1; 
    }

    *inp = buffer[inode_index];

    return 0; 
}

/**
 * TODO
 */

 int inode_indexlookup(struct unixfilesystem *fs, struct inode *inp, int blockNum) {
    if ((inp->i_mode & IALLOC) == 0) {
        return -1;
    }

    const int punteros_per_block = DISKIMG_SECTOR_SIZE / sizeof(unsigned short);

    // ───── BLOQUES DIRECTOS ─────
    if ((inp->i_mode & ILARG) == 0) {
        if (blockNum < 0 || blockNum >= 8) {
            return -1;
        }

        int sector_directo = inp->i_addr[blockNum];
        if (sector_directo == 0) {
            return -1;
        }

        return sector_directo;
    }

    // ───── BLOQUES INDIRECTOS ─────
    if (blockNum < 7 * punteros_per_block) {
        int indice_indirecto = blockNum / punteros_per_block;
        int indice_entrada = blockNum % punteros_per_block;

        int sector_indirecto = inp->i_addr[indice_indirecto];
        if (sector_indirecto == 0) {
            return -1;
        }

        unsigned short buffer_indirecto[punteros_per_block];
        if (diskimg_readsector(fs->dfd, sector_indirecto, buffer_indirecto) < 0) {
            return -1;
        }

        int sector_datos = buffer_indirecto[indice_entrada];
        if (sector_datos == 0) {
            return -1;
        }

        return sector_datos;
    }

    // ───── BLOQUES DOBLEMENTE INDIRECTOS ─────
    int sector_doble = inp->i_addr[7];
    if (sector_doble == 0) {
        return -1;
    }

    unsigned short buffer_doble[punteros_per_block];
    if (diskimg_readsector(fs->dfd, sector_doble, buffer_doble) < 0) {
        return -1;
    }

    int remaining_block = blockNum - 7 * punteros_per_block;
    int indirect_block_idx = remaining_block / punteros_per_block;
    int data_block_idx = remaining_block % punteros_per_block;
    
    unsigned short indirect_block_ptrs[punteros_per_block];
    int dbl_indirect_sector = inp->i_addr[7];
    
    if (diskimg_readsector(fs->dfd, dbl_indirect_sector, indirect_block_ptrs) == -1) return -1;
    
    int indirect_sector = indirect_block_ptrs[indirect_block_idx];
    if (indirect_sector == 0) return -1;
    
    unsigned short data_block_ptrs[punteros_per_block];
    if (diskimg_readsector(fs->dfd, indirect_sector, data_block_ptrs) == -1) return -1;
    
    int data_sector = data_block_ptrs[data_block_idx];
    if (data_sector == 0) return -1;
    
    return data_sector;
    
}

int inode_getsize(struct inode *inp) {
  return ((inp->i_size0 << 16) | inp->i_size1); 
}
