// 借助unix域socket，在消息队列中运用poll或select
// 开销：每个队列分配一个线程，每个消息额外复制两次（写入套接字、从套接字读出来）
// 使用数据报套接字而不是流套接字，保持消息边界，保证从套接字里一次只读取一条消息
// unix域数据报服务是可靠的，既不会丢失报文也不会传递出错
#include <apue.h>
#include <poll.h>
#include <pthread.h>
#include <sys/msg.h>
#include <sys/socket.h>

#define NQ 3
#define MAXMSZ 512
#define KEY 0x123

struct threadinfo {
  int qid;
  int fd;
};

struct mymesg {
  long mtype;
  char mtext[MAXMSZ];
};

void *helper(void *arg) {
  int n;
  struct mymesg m;
  struct threadinfo *tip = arg;

  for (;;) {
    memset(&m, 0, sizeof(m));
    if ((n = msgrcv(tip->qid, &m, MAXMSZ, 0, MSG_NOERROR)) < 0)
      err_sys("msgrcv error");
    if (write(tip->fd, m.mtext, n) < 0)
      err_sys("write error");
  }
}

int main() {
  int i, n, err;
  int fd[2];
  int qid[NQ];
  struct pollfd pfd[NQ];
  struct threadinfo ti[NQ];
  pthread_t tid[NQ];
  char buf[MAXMSZ];

  for (i = 0; i < NQ; i++) {
    if ((qid[i] = msgget((KEY + i), IPC_CREAT | 0666)) < 0)
      err_sys("msgget error");

    printf("queue ID %d is %d\n", i, qid[i]);
    // 创建一对无命名的、相互连接的unix域套接字
    if (socketpair(AF_UNIX, SOCK_DGRAM, 0, fd) < 0)
      err_sys("socketpair error");
    pfd[i].fd = fd[0];
    pfd[i].events = POLLIN;
    ti[i].qid = qid[i];
    ti[i].fd = fd[1];
    if ((err = pthread_create(&tid[i], NULL, helper, &ti[i])) != 0)
      err_exit(err, "pthread_create error");
  }

  for (;;) {
    if (poll(pfd, NQ, -1) < 0)
      err_sys("poll error");
    for (i = 0; i < NQ; i++) {
      if (pfd[i].revents & POLLIN) {
        if ((n = read(pfd[i].fd, buf, sizeof(buf))) < 0)
          err_sys("read error");
        buf[n] = 0;
        printf("queue id %d, message %s\n", qid[i], buf);
      }
    }
  }

  exit(0);
}