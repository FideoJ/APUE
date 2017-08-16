// 子进程修改变量不影响父进程
#include <apue.h>

int globvar = 6;
char buf[] = "a write to stdout\n";

int main() {
  int var;
  pid_t pid;
  var = 88;
  if (write(STDOUT_FILENO, buf, sizeof(buf) - 1) != sizeof(buf) - 1)
    err_sys("write error");
  // 标准输出连接到终端设备为行缓冲，否则为全缓冲。子进程继承父进程的缓冲区
  printf("before fork\n");

  if ((pid = fork()) < 0) {
    err_sys("fork error");
  } else if (pid == 0) {
    globvar++;
    var++;
  } else {
    sleep(2);
  }

  printf("pid = %ld, glob = %d, var = %d\n", (long)getpid(), globvar, var);
  exit(0);
}