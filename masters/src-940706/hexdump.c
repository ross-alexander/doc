#include <stdio.h>
#include <stdlib.h>

extern int optind;
extern char *optarg;

int main(int argc, char *argv[])
{
  FILE *f;
  unsigned char bf[256];
  int i, opt, length = 0, header = 0;

  while ((opt = getopt(argc, argv, "l:h:")) != EOF)
    switch(opt)
      {
      case 'l':
	length = atoi(optarg);
	break;
      case 'h':
	header = atoi(optarg);
	break;
      }
  if (!length) length = 16;

  if (argc - optind)
    f = fopen(argv[optind], "rb");
  else
    f = stdin;
  if (!f) exit(1);

  fread(bf, header, 1, f);
      for (i = 0; i < header; i++)
	printf("%02X ", bf[i]);
      printf("\n");

  while (fread(bf, length, 1, f))
    {
      for (i = 0; i < length; i++)
	printf("%02X ", bf[i]);
      printf("\n");
    }
  fclose(f);
}
