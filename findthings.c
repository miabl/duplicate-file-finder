#include "duplicates.h"

typedef struct _stats_found {
  int total_files;
  uint32_t total_filesize;
  int unique_files;
  uint32_t unique_filesize;
} STATS_FOUND;

void print_duplicates(HASHTABLE *ht) {
  int counter = 0;
  for (int i = 0; i < HASHTABLE_SIZE; i++) {
    LIST *current_list = ht[i];

    if (current_list != NULL) {
      while (current_list != NULL) {
        if ((current_list->count > 1) ||
            (current_list->fileinfo[0]->filecount > 1)) {
          int cnt = current_list->count;
          for (int j = 0; j < cnt; j++) {
            for (int k = 0; k < current_list->fileinfo[j]->filecount; k++) {
              printf("%s\t", current_list->fileinfo[j]->relative_filepaths[k]);
              counter++;
            }
          }
          printf("\n");
        }
        current_list = current_list->next;
      }
    }
  }
  printf("counter: %i\n", counter);
}

int count_duplicates(HASHTABLE *ht) {
  int counter = 0;
  for (int i = 0; i < HASHTABLE_SIZE; i++) {
    LIST *list = ht[i];

    if (list != NULL) {
      while (list != NULL) {
        if ((list->count > 1) || (list->fileinfo[0]->filecount > 1)) {
          for (int j = 0; j < list->count; j++) {
            for (int k = 0; k < list->fileinfo[j]->filecount; k++) {
              counter++;
            }
          }
        }
        list = list->next;
      }
    }
  }
  return counter;
}

void find_stats(HASHTABLE *ht) {
  STATS_FOUND stats = {0, 0, 0, 0};

  for (int i = 0; i < HASHTABLE_SIZE; i++) {
    LIST *list = ht[i];

    if (list != NULL) {
      while (list != NULL) {
        stats.unique_files++;
        stats.unique_filesize += list->fileinfo[0]->filesize;
        for (int j = 0; j < list->count; j++) {
          stats.total_files += list->fileinfo[j]->filecount;
          stats.total_filesize +=
              list->fileinfo[j]->filesize * list->fileinfo[j]->filecount;
        }
        list = list->next;
      }
    }
  }
  printf("%i\n%i\n%i\n%i\n", stats.total_files, stats.total_filesize,
         stats.unique_files, stats.unique_filesize);
}
