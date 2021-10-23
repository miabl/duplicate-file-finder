#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <dirent.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/types.h>

#define ADVANCED
#if defined(__linux__)
extern char *strdup(const char *s);
extern char *realpath(const char *restrict path, char *restrict resolved_path);
#endif

// A HELPFUL PREPROCESSOR MACRO TO CHECK IF ALLOCATIONS WERE SUCCESSFUL
#define CHECK_ALLOC(p)                                                         \
  if (p == NULL) {                                                             \
    perror(__func__);                                                          \
    exit(EXIT_FAILURE);                                                        \
  }

#define AUTHOR_EMAIL "miablessas@gmail.com"
#define HASHTABLE_SIZE 997

// THE HASHTABLE USES THE FILESIZE AS A KEY. EACH LIST STORES AN ARRAY OF
// FILEINFO STRUCTS. EACH ARRAY CORRESPONDS TO ONE 'UNIQUE' FILE (E.G. ALL
// DUPLICATES OF A FILE)

// DATATYPE TO STORE THE FILE INFORMATION REQUIRED BY THE PROGRAM
typedef struct _fileinfo {
  ino_t inode;
  int filecount;
  char **relative_filepaths;
  char **absolute_filepaths;
  off_t filesize;
} FILEINFO;

// DATATYPE TO STORE AN ARRAY OF FILEINFO STRUCTS
typedef struct _list {
  int count;
  struct _fileinfo **fileinfo;
  struct _list *next;
} LIST;

// RETURNS THE SHA2 HASH OF A FILE
extern char *strSHA2(char *filename);

// THESE FUNCTIONS ARE DECLARED HERE, AND DEFINED IN files.c:

// 'CREATE' A NEW, EMPTY LIST
extern LIST *list_new(void);

// ADD A NEW (FILEINFO) ITEM TO AN EXISTING LIST
extern LIST *list_add(LIST *list, char *rel_path, char *abs_path);

// DETERMINE IF A REQUIRED ITEM (A FILEINFO STRUCT) IS STORED IN A GIVEN LIST
extern LIST *list_find(LIST *list, char *filepath);

// PRINT EACH ITEM (A FILEINFO STRUCT) IN A GIVEN LIST TO stdout
extern void list_print(LIST *list);

extern void print_single_list(LIST *list);
// WE DEFINE A HASHTABLE AS A (WILL BE, DYNAMICALLY ALLOCATED) ARRAY OF LISTs
typedef LIST *HASHTABLE;

extern void print_file_names(LIST *list);

extern HASHTABLE *ht;

// THESE FUNCTIONS ARE DECLARED HERE, AND DEFINED IN hashtable.c :

// ALLOCATE SPACE FOR A NEW HASHTABLE (AND ARRAY OF LISTS)
extern HASHTABLE *hashtable_new(void);

// PROCESSES THE CONTENTS OF THE SPECIFIED DIRECTORY
extern void scan_directory(char *dirname, bool aflag);

// ADD A NEW LIST TO A GIVEN HASHTABLE
extern void hashtable_add(char *rel_path, char *abs_path, off_t filesize);

// DETERMINE IF A REQUIRED LIST ALREADY EXISTS IN A GIVEN HASHTABLE
extern bool hashtable_find(HASHTABLE *hashtable, FILEINFO *fileinfo);

// PRINT ALL NON-ZERO INDEXES OF THE SPECIFIED HASHTABLE
extern void hashtable_print();

// RETURNS SHA2 HASH OF SPECIFIED FILENAME
extern char *strSHA2(char *filename);

// PRINTS FILENAMES OF DUPLICATE FILES
extern void print_duplicates(void);

// COUNTS THE NUMBER OF DUPLICATE FILES
extern int count_duplicates(void);

// RETURNS REQUIRED STATS ABOUT THE DIRECTORY
extern void find_stats(void);

// PRINTS ALL FILES WITH MATCHING SHA2 HASHES
extern void print_matching_sha(char *sha2);

// ATTEMPTS TO MINIMIZE FILESIZE BY HARD-LINKING DUPLICATES
extern void minimize_duplicates(void);
