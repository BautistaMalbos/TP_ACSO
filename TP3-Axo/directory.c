#include "directory.h"
#include "inode.h"
#include "diskimg.h"
#include "file.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

/**
 * TODO
 */
int directory_findname(struct unixfilesystem *fs, const char *name,
    int dirinumber, struct direntv6 *dirEnt) {
if (!fs || !name || !dirEnt) return -1;

struct inode node;
if (inode_iget(fs, dirinumber, &node) < 0) {
    return -1;}

if ((node.i_mode & IALLOC) == 0) {
    return -1;}

int total_size = inode_getsize(&node);
if (total_size <= 0) {
    return -1;}

char data[DISKIMG_SECTOR_SIZE];
int logical_block = 0;
int processed_bytes = 0;

int direntry_size = sizeof(struct direntv6);
int dirname_size = 14 ;

while (processed_bytes < total_size) {
    int read = file_getblock(fs, dirinumber, logical_block, data);
    if (read <= 0) return -1;

    for (int pos = 0; pos + direntry_size <= read; pos += direntry_size) {
        struct direntv6 *entry = (struct direntv6 *)(data + pos);

        if (strncmp(entry->d_name, name, dirname_size) == 0) {
            *dirEnt = *entry;
            return 0;
        }
    }

    processed_bytes += read;
    logical_block++;
}

return -1;
}

