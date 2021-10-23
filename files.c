#include "duplicates.h"

int foundindex;
// 'CREATE' A NEW, EMPTY LIST - JUST A NULL POINTER
LIST *list_new(void) { return NULL; }

char *get_abs_path(char *relpath) {
  char *abs = realpath(relpath, NULL);
  return abs;
}

// DETERMINE IF A MATCHING INODE IS ALREADY STORED IN THE LIST
LIST *inode_find(LIST *list, char *filepath) {
  struct stat stat_info;
  stat(filepath, &stat_info);
  while (list != NULL) {
    // ITERATE THROUGH THE FILEINFO ARRAY
    for (int i = 0; i < list->count; i++) {
      // RETURN THE LIST IF A MATCHING INODE IS FOUND
      if (list->fileinfo[i]->inode == stat_info.st_ino) {
        return list;
      }
    }
    list = list->next;
  }
  // RETURN NULL IF NO MATCHING INODE IS FOUND
  return NULL;
}

// DETERMINE IF A FILE WITH A MATCHING SHA2 HASH IS ALREADY STORED IN THE LIST
LIST *sha_find(LIST *list, char *filepath) {
  while (list != NULL) {
    strSHA2(list->fileinfo[0]->absolute_filepaths[0]);

    if (list->count > 0 && list->fileinfo[0]->filecount > 0) {
      char *sha1 = strSHA2(filepath);
      char *sha2 = strSHA2(list->fileinfo[0]->absolute_filepaths[0]);
      if (strcmp(sha1, sha2) == 0) {
        return list;
      }
    }
    list = list->next;
  }
  return NULL;
}

// DETERMINE IF A DUPLICATE FILE IS ALREADY STORED IN A GIVEN LIST
// SEARCH BY INODE FIRST TO TRY TO AVOID SHA2 HASHING - CAN TAKE A LONG TIME ON
// LARGE DIRECTORIES AND FILES
//
// RETURNS THE LIST OBJECT CONTAINING DUPLICATES OF THE FILE, OR NULL IF NO
// DUPLICATES ARE FOUND
LIST *list_find(LIST *list, char *filepath) {
  LIST *original_list = list;

  struct stat statinfo;
  stat(filepath, &statinfo);
  // SEARCH BY INODE
  // printf("hardlinks: %i\n", statinfo.st_nlink);

  if (statinfo.st_nlink > 1) {
    while (list != NULL) {
      LIST *inode_list = inode_find(list, filepath);
      if (inode_list != NULL) {
        return inode_list;
      }
      list = list->next;
    }
  }

  // IF NOT FOUND BY INODE, SEARCH BY SHA
  list = original_list;
  while (list != NULL) {
    if (statinfo.st_size == list->fileinfo[0]->filesize) {
      LIST *sha_list = sha_find(list, filepath);
      if (sha_list != NULL) {
        return sha_list;
      }
    }

    list = list->next;
  }
  return NULL;
}

// ALLOCATE SPACE FOR A NEW LIST ITEM, TESTING THAT ALLOCATION SUCCEEDS
LIST *list_new_file(FILEINFO *fileinfo) {
  LIST *new = calloc(1, sizeof(LIST));
  CHECK_ALLOC(new);
  new->fileinfo = calloc(1, sizeof(fileinfo));
  new->fileinfo[0] = fileinfo;
  CHECK_ALLOC(new->fileinfo);
  new->count = 1;
  new->next = NULL;
  return new;
}

// ADD A NEW ITEM TO AN EXISTING LIST
LIST *list_add(LIST *list, char *rel_path, char *abs_path) {
  struct stat statinfo;
  stat(abs_path, &statinfo);

  // IF A MATCHING INODE OR SHA IS FOUND, RETURN THAT LIST
  LIST *found_list = list_find(list, abs_path);
  if (found_list != NULL) {
    for (int i = 0; i < found_list->count; i++) {

      // IF THE INODE IN THE FOUND LIST MATCHES THE INODE OF THE FILE, ADD THE
      // FILE TO THAT LIST
      if (statinfo.st_ino == found_list->fileinfo[i]->inode) {

        // ALLOCATE SUFFICENT SPACE IN THE RELATIVE FILEPATH ARRAY
        found_list->fileinfo[i]->relative_filepaths =
            realloc(found_list->fileinfo[i]->relative_filepaths,
                    sizeof(found_list->fileinfo[i]->relative_filepaths[0]) *
                        (found_list->fileinfo[i]->filecount + 1));

        // ADD THE NEW RELATIVE FILEPATH TO THE END OF THE ARRAY
        found_list->fileinfo[i]
            ->relative_filepaths[found_list->fileinfo[i]->filecount] = rel_path;

        // ALLOCATE SUFFICIENT SPACE IN THE ABSOLUTE FILEPATH ARRAY
        found_list->fileinfo[i]->absolute_filepaths =
            realloc(found_list->fileinfo[i]->absolute_filepaths,
                    sizeof(found_list->fileinfo[i]->absolute_filepaths[0]) *
                        (found_list->fileinfo[i]->filecount + 1));

        // ADD THE NEW ABSOLUTE FILEPATH TO THE ARRAY
        found_list->fileinfo[i]
            ->absolute_filepaths[found_list->fileinfo[i]->filecount] = abs_path;

        // INCREMENT THE FILE COUNTER
        found_list->fileinfo[i]->filecount++;

        return list;
      }
    }

    // IF THE INODE ISN'T FOUND, ADD THE FILEINFO STRUCT TO THE FILEINFO ARRAY
    found_list->fileinfo =
        realloc(found_list->fileinfo,
                sizeof(found_list->fileinfo[0]) * (found_list->count + 1));

    // ALLOCATE SPACE FOR NEW ABSOLUTE AND RELATIVE FILEPATH ARRAYS AND
    // INITIALIZE THEM WITH THE FILEPATHS
    char **relative_filepaths = calloc(1, sizeof(rel_path));
    char **absolute_filepaths = calloc(1, sizeof(abs_path));
    relative_filepaths[0] = strdup(rel_path);
    absolute_filepaths[0] = strdup(abs_path);

    // IF THE HASH EXISTS IN OUR HASHTABLE, ADD A NEW FILEINFO STRUCT TO THE
    // ARRAY
    FILEINFO *newfile = malloc(sizeof(FILEINFO));
    newfile->inode = statinfo.st_ino;
    newfile->filecount = 1;
    newfile->relative_filepaths = relative_filepaths;
    newfile->absolute_filepaths = absolute_filepaths;
    newfile->filesize = statinfo.st_size;

    // ADD THE NEW FILEINFO STRUCT TO THE END OF THE FILEPATH ARRAY & INCREMENT
    // THE COUNTER
    found_list->fileinfo[found_list->count] = newfile;
    found_list->count++;

    return list;

  } else {
    // IF DUPLICATE FILES DON'T EXIST IN THE LIST, SIMPLY ADD A NEW LIST
    char **relative_filepaths = calloc(1, sizeof(rel_path));
    char **absolute_filepaths = calloc(1, sizeof(abs_path));
    relative_filepaths[0] = strdup(rel_path);
    absolute_filepaths[0] = strdup(abs_path);

    // POPULATE A NEW FILEINFO STRUCT
    FILEINFO *newfile = malloc(sizeof(FILEINFO));
    newfile->inode = statinfo.st_ino;
    newfile->filecount = 1;
    newfile->relative_filepaths = relative_filepaths;
    newfile->absolute_filepaths = absolute_filepaths;
    newfile->filesize = statinfo.st_size;

    LIST *new = list_new_file(newfile);
    new->next = list;
    return new;
  }
}

// PRINT THE FILE NAMES STORED IN ONE NODE OF A LIST (A FILEINFO ARRAY)
void print_file_names(LIST *list) {
  if (list != NULL) {
    for (int i = 0; i < list->count; i++) {
      for (int j = 0; j < list->fileinfo[i]->filecount; j++) {
        printf("%s\n", list->fileinfo[i]->absolute_filepaths[j]);
      }
    }
  }
}
