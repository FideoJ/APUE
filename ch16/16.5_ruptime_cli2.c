#include <apue.h>
#include <errno.h>
#include <netdb.h>
#include <sys/socket.h>

#define BUFLEN 128
#define TIMEOUT 20
// 使用SIGALRM信号来防止recvfrom无限期阻塞（面向连接时，connect调用失败）
void sigalrm(int signo) {}

void print_uptime(int sockfd, struct addrinfo *aip) {
  int n;
  char buf[BUFLEN];

  buf[0] = 0;
  // 发送1字节数据通知服务器执行服务（面向连接时，连接本身已传达该信息）
  if (sendto(sockfd, buf, 1, 0, aip->ai_addr, aip->ai_addrlen) < 0)
    err_sys("sendto error");
  alarm(TIMEOUT);
  if ((n = recvfrom(sockfd, buf, BUFLEN, 0, NULL, NULL)) < 0) {
    // 非被SIGALRM中断的异常
    if (errno != EINTR)
      // If seconds is zero, any pending alarm is canceled.
      alarm(0);
    err_sys("recv error");
  }
  // 成功调用
  alarm(0);
  write(STDOUT_FILENO, buf, n);
}

int main(int argc, char *argv[]) {
  struct addrinfo *ailist, *aip;
  struct addrinfo hint;
  int sockfd, err;
  struct sigaction sa;

  if (argc != 2)
    err_quit("usage: ruptime hostname");
  sa.sa_handler = sigalrm;
  sa.sa_flags = 0;
  sigemptyset(&sa.sa_mask);
  if (sigaction(SIGALRM, &sa, NULL) < 0)
    err_sys("sigaction error");
  memset(&hint, 0, sizeof(hint));
  hint.ai_socktype = SOCK_DGRAM;
  hint.ai_canonname = NULL;
  hint.ai_addr = NULL;
  hint.ai_next = NULL;
  if ((err = getaddrinfo(argv[1], "ruptime", &hint, &ailist)) != 0)
    err_quit("getaddrinfo error: %s", gai_strerror(err));

  for (aip = ailist; aip != NULL; aip = aip->ai_next) {
    if ((sockfd = socket(aip->ai_family, SOCK_DGRAM, 0)) < 0) {
      err = errno;
    } else {
      print_uptime(sockfd, aip);
      exit(0);
    }
  }

  fprintf(stderr, "can't contact %s: %s\n", argv[1], strerror(err));
  exit(1);
}