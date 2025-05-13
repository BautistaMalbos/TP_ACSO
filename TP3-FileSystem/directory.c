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
if (fs == NULL || name == NULL || dirEnt == NULL) {
return -1;
}

if (strlen(name) == 0 || strlen(name) > 14) {
return -1;
}

struct inode in;
if (inode_iget(fs, dirinumber, &in) == -1) {
return -1;
}

if ((in.i_mode & IALLOC) == 0) {
return -1;
}

int num_entries = in.i_size0 / sizeof(struct direntv6);
for (int i = 0; i < num_entries; i++) {
struct direntv6 entry;
if (directory_getentry(fs, dirinumber, i, &entry) == -1) {
return -1;
}

if (strncmp(entry.d_name, name, 14) == 0) {
memcpy(dirEnt, &entry, sizeof(struct direntv6));
return 0;
}
}

return -1;
}

int directory_getentry(struct unixfilesystem *fs, int dirinumber,
  int entrynum, struct direntv6 *dirEnt) {
if (fs == NULL || dirEnt == NULL) {
return -1;
}

struct inode in;
if (inode_iget(fs, dirinumber, &in) == -1) {
return -1;
}

if ((in.i_mode & IALLOC) == 0) {
return -1;
}

int num_entries = in.i_size0 / sizeof(struct direntv6);
if (entrynum >= num_entries) {
return -1;
}

int offset = entrynum * sizeof(struct direntv6);

int block_num = inode_indexlookup(fs, &in, offset);
if (block_num < 0) {
return -1;
}

char block_data[DISKIMG_SECTOR_SIZE];
if (diskimg_readsector(fs->dfd, block_num, block_data) < 0) {
return -1;
}

int offset_in_block = offset % DISKIMG_SECTOR_SIZE;
memcpy(dirEnt, block_data + offset_in_block, sizeof(struct direntv6));

return 0;
}
