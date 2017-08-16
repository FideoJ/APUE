// 通过TELL/WAIT机制解决竞争条件
#include <apue.h>

static void charatatime(char *);

int main() {
  pid_t pid;

  TELL_WAIT();

  if ((pid = fork()) < 0) {
    err_sys("fork error");
  } else if (pid == 0) {
    // WAIT_PARENT();
    charatatime("output from child\n");
    TELL_PARENT(getppid());
  } else {
    WAIT_CHILD();
    charatatime("output from parent\n");
    // TELL_CHILD(pid);
  }
  exit(0);
}

static void charatatime(char *str) {
  char *ptr;
  int c;
  // 设置标准输出为不带缓冲，使内核尽可能多次在两个进程间切换，演示竞争条件
  setbuf(stdout, NULL);
  for (ptr = str; (c = *ptr++) != 0;)
    putc(c, stdout);
}