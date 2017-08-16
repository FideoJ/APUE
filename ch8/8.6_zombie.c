// 双层fork，使子进程之子进程成为孤儿进程
#include <apue.h>
#include <sys/wait.h>

int main() {
  pid_t pid;
  if ((pid = fork()) < 0) {
    err_sys("fork error");
  } else if (pid == 0) {
    if ((pid = fork()) < 0)
      err_sys("fork error");
    else if (pid > 0)
      exit(0);
    sleep(2);
    // 孤儿进程被init或upstart（非1号进程）等进程收养
    printf("second child, parent pid = %ld\n", (long)getppid());
    exit(0);
  }
  if (waitpid(pid, NULL, 0) != pid)
    err_sys("waitpid error");
  exit(0);
}