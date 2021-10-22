#include "duplicates.h"
#include <getopt.h>

#if defined(ADVANCED)
#define OPTLIST "aAf:h:lmq"
#else
#define OPTLIST "aAf:h:lq"
#endif

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
  char *progname = (progname = strrchr(argv[0], '/')) ? progname + 1 : argv[0];
  // int nduplicates = 0;

  bool aflag = false; // all files
                      //  char *fflag = NULL; // find matching files
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
      printf("-a list all files\n");
      break;

      // REPORT ADVANCED ATTEMPT
    case 'A':
      printf("-A report advanced\n");
#if defined(ADVANCED)
      exit(EXIT_SUCCESS);
#else
      exit(EXIT_FAILURE);
#endif
      break;

      // FIND DUPLICATE FILES WITH THIS PROVIDED HASH
    case 'h':
      printf("-h compare to hash");
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
  }
#else
  // ENSURE THAT WE CAN ACCESS OUR REQUESTED DIRECTORY
  if (stat(argv[0], &statinfo) != 0 || !S_ISDIR(statinfo.st_mode)) {
    perror(argv[0]);
    exit(EXIT_FAILURE);
  }
#endif
printf("before processing\n");
  HASHTABLE *ht = process_directory(argv[0], aflag);
  printf("processed hashtable\n");
  // nduplicates = count_duplicates(ht);

  // EXIT QUIETLY BY ONLY PROVIDING EXIT STATUS (SUCCESS IF NO DUPLICATES)
  if (qflag) {
    exit(count_duplicates(ht) == 0 ? EXIT_SUCCESS : EXIT_FAILURE);
  }

  if (lflag) {
    print_duplicates(ht);
  }

  // NO ARGUMENTS INPUTTED, PRINT DEFAULT STATISTICS
  else {
    find_stats(ht);
  }

  // hashtable_print(ht);

  return 0;
}
