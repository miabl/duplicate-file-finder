#include "duplicates.h"
#include <getopt.h>

#if defined(ADVANCED)
#define OPTLIST "aAf:h:lmq"
#else
#define OPTLIST "aAf:h:lq"
#endif

HASHTABLE *ht;
// REPORT ERROS WITH COMMAND-LINE OPTIONS TO stderr

void usage(char *progname) {
#if defined(ADVANCED)
  fprintf(stderr, "Usage: %s [options] dirname1 [dirname2 ...]\n", progname);
#else
  fprintf(stderr, "Usage %s [options] dirname\n", progname);
#endif
  fprintf(stderr, "where options are :\n");

  fprintf(stderr,
          "   -a\t\tcheck all files, including those begging with '.'\n");
  fprintf(stderr,
          "   -A\t\treport if this program attempts the advanced features\n");
  fprintf(stderr, "   -l\t\tlist all duplicates found\n");
#if defined(ADVANCED)
  fprintf(stderr, "   -m\t\tminimize duplicates by creating hard-links\n");
#endif
  fprintf(stderr, "   -q\t\tquietly test if any duplicates exist\n");

  fprintf(stderr, "\nPlease report any bugs to %s\n", AUTHOR_EMAIL);
  exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {

  ht = calloc(HASHTABLE_SIZE, sizeof(LIST *));
  char *progname = (progname = strrchr(argv[0], '/')) ? progname + 1 : argv[0];
  // int nduplicates = 0;

  bool aflag = false; // all files
  char *fflag = NULL; // find matching files
  char *hflag = NULL; // find matching files
  bool lflag = false; // list duplicates
  bool qflag = false; // test for duplicates

#if defined(ADVANCED)
  bool mflag = false;
#endif

  int opt;

  opterr = 0;

  while ((opt = getopt(argc, argv, OPTLIST)) != -1) {
    switch (opt) {
    // CHECK ALL FILES
    case 'a':
      aflag = !aflag;
      break;

      // REPORT ADVANCED ATTEMPT
    case 'A':
#if defined(ADVANCED)
      exit(EXIT_SUCCESS);
#else
      exit(EXIT_FAILURE);
#endif
      break;

    case 'f':
      fflag = strdup(optarg);
      CHECK_ALLOC(fflag);
      break;

      // FIND DUPLICATE FILES WITH THIS PROVIDED HASH
    case 'h':
      hflag = strdup(optarg);
      CHECK_ALLOC(hflag);
      break;

      // LIST FILENAMES OF FOUND DUPLICATES
    case 'l':
      lflag = !lflag;
      break;

      // MINIMIZE FILESIZE BY CREATING HARD LINKS
#if defined(ADVANCED)
    case 'm':
      mflag = !mflag;
      break;
#endif

      // QUIETLY TEST FOR DUPLICATES
    case 'q':
      printf("-q execute quietly\n");
      qflag = !qflag;
      break;

    default:
      fprintf(stderr, "%s : illegal option -%c\n", progname, optopt);
      argc = -1;
      break;
    }
  }

  argv += optind;
  argc -= optind;

  // ENSURE THAT WE HAVE THE REQUIRED NUMBER OF DIRECTORIES

#if defined(ADVANCED)
  if (argc < 1) {
    usage(progname);
  }
#else
  if (argc != 1) {
    usage(progname);
  }
#endif

  struct stat statinfo;

#if defined(ADVANCED)
  for (int a = 0; a < argc; ++a) {
    // ENSURE THAT WE CAN ACCESS EACH REQUESTED DIRECTORY
    if (stat(argv[a], &statinfo) != 0 || !S_ISDIR(statinfo.st_mode)) {
      perror(argv[a]);
      exit(EXIT_FAILURE);
    }
    scan_directory(argv[a], aflag);
  }
#else
  // ENSURE THAT WE CAN ACCESS OUR REQUESTED DIRECTORY
  if (stat(argv[0], &statinfo) != 0 || !S_ISDIR(statinfo.st_mode)) {
    perror(argv[0]);
    exit(EXIT_FAILURE);
  }
  scan_directory(argv[0], aflag);
  printf("%s scanned\n", argv[0]);
#endif
  // nduplicates = count_duplicates(ht);

#if defined(ADVANCED)

  if (mflag) {
    minimize_duplicates();
    ht = hashtable_new();
    for (int a = 0; a < argc; ++a) {
      // ENSURE THAT WE CAN ACCESS EACH REQUESTED DIRECTORY
      if (stat(argv[a], &statinfo) != 0 || !S_ISDIR(statinfo.st_mode)) {
        perror(argv[a]);
        exit(EXIT_FAILURE);
      }
      scan_directory(argv[a], aflag);
    }
    hashtable_print();
  }
#endif
  // EXIT QUIETLY BY ONLY PROVIDING EXIT STATUS (SUCCESS IF NO DUPLICATES)

  if (fflag) {
    char *sha2 = strSHA2(fflag);
    if (sha2 == NULL) {
      printf("sha could not be produced for %s\nplease check the filepath is "
             "correct and the file exists on your system\n",
             fflag);
      exit(EXIT_FAILURE);
    }
    print_matching_sha(sha2);
    free(sha2);
    free(fflag);
  } else if (hflag) {
    print_matching_sha(hflag);
    free(hflag);
  }

  if (qflag) {
    exit(count_duplicates() == 0 ? EXIT_SUCCESS : EXIT_FAILURE);
  }

  else if (lflag) {
    print_duplicates();
  }

  // NO ARGUMENTS INPUTTED, PRINT DEFAULT STATISTICS
  else {
    find_stats();
  }

  // hashtable_print(ht);

  return 0;
}
