/* This program removes wtmp entries by name or tty number */

#include <utmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <sys/file.h>
#include <sys/fcntl.h>

/**
 * The memset() function is preferred over this function.
 *   For maximum portability, it is recommended to replace the function call
 *   to bzero() as follows:
 */
#define bzero(b,len) (memset((b), '\0', (len)), (void) 0)

void usage(char *name)
{
  printf("Usage: %s [ --user=user | --tty=tty | --host=host ] wtmp_filename\n", name);
  exit(1);
}

void main (int argc, char *argv[])
{
  struct utmp utmp;

  int size, fd, x = 0;
  char *wtmp_filename = "";
  char *user = "";
  char *tty = "";
  char *host = "";

  /* getopt_long stores the option index here. */
  int option_index = 0;

  static struct option long_options[] =
  {
    {"user", required_argument, 0, 'u'},
    {"tty",  required_argument, 0, 't'},
    {"host", required_argument, 0, 'h'},
    {0, 0, 0, 0}
  };

  int c;
  while ((c = getopt_long(argc, argv, "u:t:h:", long_options, &option_index)) != -1)
  {
    switch (c)
    {
      case 'u':
        user = optarg;
        break;

      case 't':
        tty = optarg;
        break;

      case 'h':
        host = optarg;
        break;

      case '?':
        usage(argv[0]);
        break;

      default:
        abort();
    }
  }

  /* One of options should be assigned! */
  if (user=="" && tty=="" && host=="")
    usage(argv[0]);

  if (user!="" && strlen(user)<2) {
    printf("Error: Username is too short\n");
    exit(1);
  }

  /* non-argument value shouldb be one */
  if (!(argc - optind) == 1)
    usage(argv[0]);

  int index;
  for (index = optind; index < argc; index++)
      wtmp_filename = argv[index];

  if ((fd = open(wtmp_filename, O_RDWR))==-1) {
    printf("Error: Open on %s\n", wtmp_filename);
    exit(1);
  }

  /* do som ouutput with search criterions */
  printf("[Searching...");
  if (user!="")
    printf(" user=`%s'", user);
  if (tty!="")
    printf(" tty=`%s'", tty);
  if (host!="")
    printf(" host=`%s'\n", host);
  printf("]: ");

  if (fd >= 0)
  {
    size = read(fd, &utmp, sizeof(struct utmp));
    while (size == sizeof(struct utmp))
    {
      /* utmp record should satisfy all criterions */
      if (   (tty  == "" || !strcmp(utmp.ut_line, tty))
          && (user == "" || !strcmp(utmp.ut_name, user))
          && (host == "" ||  strstr(utmp.ut_host, host)) )
      {
        lseek(fd, -sizeof(struct utmp), L_INCR);
        bzero(&utmp, sizeof(struct utmp));
        write(fd, &utmp, sizeof(struct utmp));
        x++;
      }
      size = read( fd, &utmp, sizeof(struct utmp) );
    }
  }

  /* print some summary */
  if (!x)
    puts("No entries found.");
  else
    printf("%d entries removed.\n", x);

  close(fd);
  exit(0);
}
