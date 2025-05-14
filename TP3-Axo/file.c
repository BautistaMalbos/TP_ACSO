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
    struct inode in;

    if (inode_iget(fs, inumber, &in) == -1)
        return -1;

    if ((in.i_mode & IALLOC) == 0)
        return -1;

    int sector = inode_indexlookup(fs, &in, blockNum);
    if (sector <= 0)
        return 0; 

    int bytes_read = diskimg_readsector(fs->dfd, sector, buf);
    if (bytes_read == -1)
        return -1;

    int filesize = in.i_size0 | (in.i_size1 << 16);
    int total_blocks = (filesize + DISKIMG_SECTOR_SIZE - 1) / DISKIMG_SECTOR_SIZE;

    if (blockNum < total_blocks - 1) {
        return DISKIMG_SECTOR_SIZE;
    } 
    else if (blockNum == total_blocks - 1) {
        int resto = filesize % DISKIMG_SECTOR_SIZE;
        if (resto == 0) {
            return DISKIMG_SECTOR_SIZE;
        } else {
            return resto;
        }
    } 
   
    return 0;
    
}



