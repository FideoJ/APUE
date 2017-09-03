#include <apue.h>
#include <pwd.h>

static void my_alarm(int signo) {
  // signal(SIGALRM, my_alarm);
  struct passwd *rootptr;

  printf("in signal handler\n");
  if ((rootptr = getpwnam("root")) == NULL)
    err_sys("getpwnam(root) error");
  alarm(1);
}

int main() {
  struct passwd *ptr;

  signal(SIGALRM, my_alarm);
  alarm(1);
  for (;;) {
    if ((ptr = getpwnam("linjian")) == NULL)
      err_sys("getpwnam error");
    if (strcmp(ptr->pw_name, "linjian") != 0)
      printf("return value corrupted!, pw_name = %s\n", ptr->pw_name);
  }
}