#include "duplicates.h"

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
    for (int i = 0; i < list->count; i++) {
      if (list->fileinfo[i]->inode == stat_info.st_ino) {
        return list;
      }
    }
    list = list->next;
  }
  return NULL;
}

// DETERMINE IF A FILE WITH A MATCHING SHA2 HASH IS ALREADY STORED IN THE LIST
LIST *sha_find(LIST *list, char *filepath) {
  if (list != NULL) {
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
  new->count = 1;
  CHECK_ALLOC(new->fileinfo);
  new->next = NULL;
  return new;
}

// ADD A NEW ITEM TO AN EXISTING LIST
LIST *list_add(LIST *list, char *rel_path, char *abs_path) {
  struct stat statinfo;
  stat(abs_path, &statinfo);

  LIST *found_list = list_find(list, abs_path);
  if (list != NULL && found_list != NULL) {
    for (int i = 0; i < found_list->count; i++) {
      FILEINFO *info = found_list->fileinfo[i];
      if (statinfo.st_ino == info->inode) {
        info->relative_filepaths = realloc(info->relative_filepaths,
                                           sizeof(info->relative_filepaths[0]) *
                                               (info->filecount + 1));
        info->relative_filepaths[info->filecount] = rel_path;
        info->absolute_filepaths = realloc(info->absolute_filepaths,
                                           sizeof(info->absolute_filepaths[0]) *
                                               (info->filecount + 1));
        info->absolute_filepaths[info->filecount] = abs_path;
        info->filecount++;
        return found_list;
      }
    }

    // IF THE INODE ISN'T FOUND, ADD THE FILEINFO STRUCT TO THE FILEINFO ARRAY
    list->fileinfo =
        realloc(list->fileinfo, sizeof(list->fileinfo[0]) * (list->count + 1));

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

    list->fileinfo[list->count] = newfile;
    list->count++;

    return list;

  } else {
    // make struct
    char **relative_filepaths = calloc(1, sizeof(rel_path));
    char **absolute_filepaths = calloc(1, sizeof(abs_path));
    relative_filepaths[0] = strdup(rel_path);
    absolute_filepaths[0] = strdup(abs_path);

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

// PRINT EACH INODE & FILEPATH IN A LIST TO stdout
void list_print(LIST *list) {
  if (list != NULL) {
    while (list != NULL) {
      printf("list count: %i\n", list->count);
      for (int i = 0; i < list->count; i++) {
        printf("----------------------fileinfo[%i]---------------\n", i);
        printf("\tfilecount: %i\n", list->fileinfo[i]->filecount);
        printf("\tinode: %lu\n", list->fileinfo[i]->inode);
        printf("\trelative_filepaths:\n");
        for (int j = 0; j < list->fileinfo[i]->filecount; j++) {
          printf("\t\t%s\n", list->fileinfo[i]->absolute_filepaths[j]);
        }
        printf("\tabsolute_filepaths:\n");
        for (int j = 0; j < list->fileinfo[i]->filecount; j++) {
          printf("\t\t%s\n", list->fileinfo[i]->absolute_filepaths[j]);
        }
      }
      list = list->next;
    }
  } else {
    printf("list was null\n");
  }
}
