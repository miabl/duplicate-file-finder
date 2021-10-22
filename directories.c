#include "duplicates.h"

void scan_directory(char *dirname, bool aflag, HASHTABLE *ht) {
  DIR *dirp;
  struct dirent *dp;

  dirp = opendir(dirname);

  if (dirp == NULL) {
    printf("failed to read %s\n", dirname);
  }
  // READ FROM THE REQUIRED DIRECTORY IF WE CAN READ IT, UNTIL WE REACH ITS END
  while (dirp != NULL && (dp = readdir(dirp)) != NULL) {
    char *dirpath = realpath(dirname, NULL);
    struct stat stat_info;

    // FIND THE RELATIVE PATHNAME FOR THE FILE
    char pathname[MAXPATHLEN];
    sprintf(pathname, "%s/%s", dirname, dp->d_name);
    char *filepath = realpath(pathname, NULL);

    // PROCESS THE REQUIRED FILES DEPENDING ON -a OPTION
    if ((!aflag && dp->d_name[0] != '.') |
        (filepath != NULL && (aflag && strcmp(dirpath, filepath) < 0))) {
      stat(filepath, &stat_info);
      if (!S_ISLNK(stat_info.st_mode)) {
        // IF THE FILE IS A DIRECTORY, RECURSIVELY CALL scan_directory()
        if (S_ISDIR(stat_info.st_mode)) {
          scan_directory(pathname, aflag, ht);
        } else {
          hashtable_add(ht, pathname, filepath, stat_info.st_size);
          // hashtable_print(ht);
        }
      }
    }
  }

  // CLOSE THE DIRECTORY
  closedir(dirp);
}

HASHTABLE *process_directory(char *dirname, bool aflag) {
  HASHTABLE *ht = hashtable_new();
  scan_directory(dirname, aflag, ht);
  // hashtable_print(ht);
  return ht;
}

// ALLOCATES MEMORY FOR A NEW (INITIALLY EMPTY) HASHTABLE

HASHTABLE *hashtable_new(void) {
  HASHTABLE *new = calloc(HASHTABLE_SIZE, sizeof(LIST *));

  CHECK_ALLOC(new);
  return new;
}

// ADD A NEW FILE TO A GIVEN HASHTABLE
void hashtable_add(HASHTABLE *hashtable, char *rel_path, char *abs_path,
                   off_t filesize) {
  uint32_t h = filesize % HASHTABLE_SIZE;
  hashtable[h] = list_add(hashtable[h], rel_path, abs_path);
}

// DETERMINE IF A FILE ALREADY HAS AN ENTRY IN THE GIVEN HASHTABLE
bool hashtable_find(HASHTABLE *hashtable, FILEINFO *fileinfo) {
  uint32_t h = fileinfo->filesize % HASHTABLE_SIZE;

  return list_find(hashtable[h], fileinfo->absolute_filepaths[0]);
}

// PRINTS ALL NON-NULL INDEXES OF THE HASHTABLE
void hashtable_print(HASHTABLE *ht) {
  printf("------------------------HASHTABLE-------------------------\n");
  for (int i = 0; i < HASHTABLE_SIZE; i++) {
    if (ht[i] != NULL) {
      printf("ht[%i]:\n", i);
      list_print(ht[i]);
    }
  }
  printf("----------------------------------------------------------\n");
}