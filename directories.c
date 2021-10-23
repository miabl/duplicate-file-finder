#include "duplicates.h"

void scan_directory(char *dirname, bool aflag) {
  DIR *dirp;
  struct dirent *dp;

  dirp = opendir(dirname);

  // READ FROM THE REQUIRED DIRECTORY IF WE CAN READ IT, UNTIL WE REACH ITS END
  while (dirp != NULL && (dp = readdir(dirp)) != NULL) {
    char *dirpath = realpath(dirname, NULL);
    struct stat stat_info;

    // FIND THE RELATIVE PATHNAME FOR THE FILE
    char pathname[MAXPATHLEN];
    sprintf(pathname, "%s/%s", dirname, dp->d_name);
    char *filepath = realpath(pathname, NULL);

    // PROCESS THE REQUIRED FILES DEPENDING ON -a OPTION
    if (filepath != NULL && ((!aflag && dp->d_name[0] != '.') |
                             (aflag && strcmp(dirpath, filepath) < 0))) {

      stat(filepath, &stat_info);

      // CHECK THAT THE FILE IS NOT A SYMBOLIC LINK
      if (!S_ISLNK(stat_info.st_mode)) {
        // IF THE FILE IS A DIRECTORY, RECURSIVELY CALL scan_directory()
        if (S_ISDIR(stat_info.st_mode)) {
          scan_directory(pathname, aflag);
        } else {
          hashtable_add(pathname, filepath, stat_info.st_size);
        }
      }
    }
  }

  // CLOSE THE DIRECTORY
  closedir(dirp);
}

// ALLOCATES MEMORY FOR A NEW (INITIALLY EMPTY) HASHTABLE

HASHTABLE *hashtable_new(void) {
  HASHTABLE *new = calloc(HASHTABLE_SIZE, sizeof(LIST *));

  CHECK_ALLOC(new);
  return new;
}

// ADD A NEW FILE TO A GIVEN HASHTABLE
void hashtable_add(char *rel_path, char *abs_path, off_t filesize) {
  uint32_t h = filesize % HASHTABLE_SIZE;
  ht[h] = list_add(ht[h], rel_path, abs_path);
}

// DETERMINE IF A FILE ALREADY HAS AN ENTRY IN THE GIVEN HASHTABLE
bool hashtable_find(HASHTABLE *hashtable, FILEINFO *fileinfo) {
  uint32_t h = fileinfo->filesize % HASHTABLE_SIZE;

  return list_find(hashtable[h], fileinfo->absolute_filepaths[0]);
}
