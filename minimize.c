#include "duplicates.h"

FILEINFO *minimized = NULL;
int mincount = 0;

// RETURNS THE INDEX OF THE FILEINFO STRUCT WITH THE MOST FILEPATHS
int most_filepaths(LIST *list) {
  FILEINFO largest = *list->fileinfo[0];
  int index = 0;
  for (int i = 0; i < list->count; i++) {
    if (largest.filecount < list->fileinfo[i]->filecount) {
      largest = *list->fileinfo[i];
      index = i;
    }
  }
  return index;
}

FILEINFO *consolidate_files(LIST *list, int largest, int h) {
  for (int i = 0; i < list->count; i++) {
    if (list->fileinfo[i]->inode != list->fileinfo[largest]->inode) {
      for (int j = 0; j < list->fileinfo[i]->filecount; j++) {
        unlink(list->fileinfo[i]->absolute_filepaths[j]);
        link(list->fileinfo[largest]->absolute_filepaths[0],
             list->fileinfo[i]->absolute_filepaths[j]);
        hashtable_add(list->fileinfo[i]->absolute_filepaths[j],
                      list->fileinfo[i]->relative_filepaths[j],
                      list->fileinfo[largest]->filesize);
      }
    }
  }
  return (list->fileinfo[largest]);
}

void minimize_duplicates() {
  for (int i = 0; i < HASHTABLE_SIZE; i++) {
    LIST *list = ht[i];
    if (list != NULL) {
      while (list != NULL) {
        int index = most_filepaths(list);
        minimized = realloc(minimized, sizeof(FILEINFO) * (mincount + 1));
        minimized[mincount] = *consolidate_files(list, index, i);

        list = list->next;
      }
    }
  }
}
