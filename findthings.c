#include "duplicates.h"

typedef struct _stats_found {
  int total_files;
  uint32_t total_filesize;
  int unique_files;
  uint32_t unique_filesize;
} STATS_FOUND;

#if defined(ADVANCED)
void print_duplicates() {
  for (int i = 0; i < HASHTABLE_SIZE; i++) {
    LIST *current_list = ht[i];

    if (current_list != NULL) {
      while (current_list != NULL) {
        if (current_list->count > 1) {
          list_print(current_list);
          for (int j = 0; j < current_list->count; j++) {
            for (int k = 0; k < current_list->fileinfo[j]->filecount; k++) {
              printf("%s\t", current_list->fileinfo[j]->relative_filepaths[k]);
            }
          }
          printf("\n");
        }
        current_list = current_list->next;
      }
    }
  }
}
#else
void print_duplicates() {
  for (int i = 0; i < HASHTABLE_SIZE; i++) {
    LIST *current_list = ht[i];

    if (current_list != NULL) {
      while (current_list != NULL) {
        if ((current_list->count > 1) ||
            (current_list->fileinfo[0]->filecount > 1)) {
          for (int j = 0; j < current_list->count; j++) {
            for (int k = 0; k < current_list->fileinfo[j]->filecount; k++) {
              printf("%s\t", current_list->fileinfo[j]->relative_filepaths[k]);
            }
          }
          printf("\n");
        }
        current_list = current_list->next;
      }
    }
  }
}
#endif

int count_duplicates() {
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

void find_stats() {
  STATS_FOUND stats = {0, 0, 0, 0};

  for (int i = 0; i < HASHTABLE_SIZE; i++) {
    LIST *list = ht[i];

    if (list != NULL) {
      while (list != NULL) {
        // list_print(list);
        stats.unique_files++;
        stats.unique_filesize += list->fileinfo[0]->filesize;
        for (int j = 0; j < list->count; j++) {
#if defined(ADVANCED)
          stats.total_filesize += list->fileinfo[j]->filesize;
          stats.total_files += 1;
#else
          stats.total_filesize +=
              list->fileinfo[j]->filesize * list->fileinfo[j]->filecount;
          stats.total_files += list->fileinfo[j]->filecount;
#endif
        }
        list = list->next;
      }
    }
  }
  printf("%i\n%i\n%i\n%i\n", stats.total_files, stats.total_filesize,
         stats.unique_files, stats.unique_filesize);
}

void print_matching_sha(char *sha2) {
  for (int i = 0; i < HASHTABLE_SIZE; i++) {
    LIST *list = ht[i];
    if (list != NULL) {
      while (list != NULL) {
        char *list_sha2 = strSHA2(list->fileinfo[0]->absolute_filepaths[0]);
        if (strcmp(sha2, list_sha2) == 0) {
          for (int j = 0; j < list->count; j++) {
            for (int k = 0; k < list->fileinfo[j]->filecount; k++) {
              printf("%s\n", list->fileinfo[j]->relative_filepaths[k]);
            }
          }
        }
        list = list->next;
      }
    }
  }
}
