// 几类进程终止状态
#include <apue.h>
#include <sys/wait.h>

void pr_exit(int status) {
  // 正常终止
  if (WIFEXITED(status))
    printf("normal termination, exit status = %d\n", WEXITSTATUS(status));
  // 异常终止（接到一个不捕捉的信号）
  else if (WIFSIGNALED(status))
    printf("abnormal termination, signal number = %d%s\n", WTERMSIG(status),
// core dump文件，当设置ulimit -c unlimited等时产生，可用gdb a.out core调试
#ifdef WCOREDUMP
           WCOREDUMP(status) ? " (core file generated)" : "");
#else
           "");
#endif
  // 作业控制
  else if (WIFSTOPPED(status))
    printf("child stopped, signal number = %d\n", WSTOPSIG(status));
}

int main() {
  pid_t pid;
  int status;

  if ((pid = fork()) < 0)
    err_sys("fork error");
  else if (pid == 0)
    exit(7);

  if (wait(&status) != pid)
    err_sys("wait error");
  pr_exit(status);

  if ((pid = fork()) < 0)
    err_sys("fork error");
  else if (pid == 0)
    abort();

  if (wait(&status) != pid)
    err_sys("wait error");
  pr_exit(status);

  if ((pid = fork()) < 0)
    err_sys("fork error");
  else if (pid == 0)
    status /= 0;

  if (wait(&status) != pid)
    err_sys("wait error");
  pr_exit(status);

  exit(0);
}