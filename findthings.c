#include "duplicates.h"

typedef struct _stats_found {
  int total_files;
  uint32_t total_filesize;
  int unique_files;
  uint32_t unique_filesize;
} STATS_FOUND;


// PRINT DUPLICATE FILES
// FOR THE ADVANCED VERSION, ONLY PRINTS ONE FILENAME ASSOCIATED WITH EACH INODE
#if defined(ADVANCED)
void print_duplicates() {
  hashtable_print();
  for (int i = 0; i < HASHTABLE_SIZE; i++) {
    LIST *list = ht[i];

      while (list != NULL) {

        // THERE ARE ONLY NON-HARDLINKED DUPLICATES IF COUNT > 1
        if (list->count > 1) {
          for (int j = 0; j < list->count; j++) {
printf("%s\t", list->fileinfo[j]->relative_filepaths[0]);
          }
          printf("\n");
        }
        list = list->next;
      }
  }
}
#else
void print_duplicates() {
  for (int i = 0; i < HASHTABLE_SIZE; i++) {
    LIST *list = ht[i];

      while (list != NULL) {
        if ((list->count > 1) ||
            (list->fileinfo[0]->filecount > 1)) {
          for (int j = 0; j < list->count; j++) {
            for (int k = 0; k < list->fileinfo[j]->filecount; k++) {
              printf("%s\t", list->fileinfo[j]->relative_filepaths[k]);
            }
          }
          printf("\n");
        }
        list = list->next;
      }
  }
}
#endif

// COUNTS THE NUMBER OF DUPLICATE FILES ENCOUNTERED
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

// RETURNS THE NUMBER AND SIZE OF ALL/UNIQUE FILES
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

          // FOR THE ADVANCED VERSION, ONLY COUNT ONCE FOR EACH FILEINFO STRUCT IN THE ARRAY
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

// PRINTS ALL FILES MATCHING THE INPUTTED SHA2 STRING
void print_matching_sha(char *sha2) {
  hashtable_print();
  for (int i = 0; i < HASHTABLE_SIZE; i++) {
    LIST *list = ht[i];
      while (list != NULL) {
        printf("before sha\n");
        char *list_sha2 = strSHA2(list->fileinfo[0]->absolute_filepaths[0]);
        if (strcmp(sha2, list_sha2) == 0) {
          for (int j = 0; j < list->count; j++) {
            for (int k = 0; k < list->fileinfo[k]->filecount; k++) {
              printf("j = %i, k = %i\n", j, k);
              printf("%s\n", list->fileinfo[j]->relative_filepaths[k]);
              printf("after print\n");
            }
          }
        }
        list = list->next;
        printf("next list\n");
    }
  }
}
