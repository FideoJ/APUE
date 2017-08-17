#include <apue.h>
#define BUFFSIZE 4096

void cp_with_read() {
  int n;
  char buf[BUFFSIZE];

  while ((n = read(STDIN_FILENO, buf, BUFFSIZE)) > 0)
    if (write(STDOUT_FILENO, buf, n) != n)
      err_sys("write error");

  if (n < 0)
    err_sys("read error");
}

void cp_with_getc() {
  int c;

  while ((c = getc(stdin)) != EOF)
    if (putc(c, stdout) == EOF)
      err_sys("output error");

  if (ferror(stdin))
    err_sys("input error");
}

void cp_with_fgets() {
  char buf[MAXLINE];

  while (fgets(buf, MAXLINE, stdin) != NULL)
    if (fputs(buf, stdout) == EOF)
      err_sys("output error");

  if (ferror(stdin))
    err_sys("input error");
}
