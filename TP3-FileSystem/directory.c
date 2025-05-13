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
  //Implement your code here
  if (fs == NULL || name == NULL || dirEnt == NULL) {
    return -1; // Invalid input
  }
  if (strlen(name) == 0 || strlen(name) > 14) {
    return -1; // Invalid name length
  }
  struct inode in;
  if (inode_iget(fs, dirinumber, &in) == -1) {
    return -1; // Error getting inode
  }
  if ((in.i_mode & IALLOC) == 0) {
    return -1; // Inode not allocated
  }
  int num_entries = in.i_size0 / sizeof(struct direntv6);
  for (int i = 0; i < num_entries; i++) {
    struct direntv6 entry;
    if (directory_getentry(fs, dirinumber, i, &entry) == -1) {
      return -1; // Error getting directory entry
    }
    if (strcmp(entry.d_name, name) == 0) {
      memcpy(dirEnt, &entry, sizeof(struct direntv6));
      return 0; // Entry found
    }
  }
  return -1; // Entry not found
}


int directory_getentry(struct unixfilesystem *fs, int dirinumber,
    int entrynum, struct direntv6 *dirEnt) {

  struct inode in;

  if (inode_iget(fs, dirinumber, &in) == -1) {
    return -1; // Error al obtener el inodo
  }

  if ((in.i_mode & IALLOC) == 0) {
    return -1; // Inodo no asignado
  }

  int num_entries = in.i_size0 / sizeof(struct direntv6);
  if (entrynum >= num_entries) {
    return -1; // Entry number out of range
  }
  int sector_num = INODE_START_SECTOR + (dirinumber - 1) / (DISKIMG_SECTOR_SIZE / sizeof(struct inode));
  int inode_index = (dirinumber - 1) % (DISKIMG_SECTOR_SIZE / sizeof(struct inode));
  struct direntv6 entries[DISKIMG_SECTOR_SIZE / sizeof(struct direntv6)];
  int bytes_read = diskimg_readsector(fs->dfd, sector_num, entries);
  if (bytes_read == -1) {
    return -1; // Error reading sector
  }
  memcpy(dirEnt, &entries[entrynum], sizeof(struct direntv6));
  return bytes_read;
}