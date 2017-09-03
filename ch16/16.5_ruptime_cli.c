#include <apue.h>
#include <errno.h>
#include <netdb.h>
#include <sys/socket.h>

#define BUFLEN 128

extern int connect_retry(int, int, int, const struct sockaddr *, socklen_t);

void print_uptime(int sockfd) {
  int n;
  char buf[BUFLEN];
  // SOCK_STREAM套接字不能保证调用一次recv就会读取整个字符串，需要重复调用直到它返回0
  while ((n = recv(sockfd, buf, BUFLEN, 0)) > 0)
    write(STDOUT_FILENO, buf, n);
  if (n < 0)
    err_sys("recv error");
}

int main(int argc, char *argv[]) {
  struct addrinfo *ailist, *aip;
  struct addrinfo hint;
  int sockfd, err;

  if (argc != 2)
    err_quit("usage: ruptime hostname");
  // hint是一个用于过滤地址的模板，包括ai_family、ai_flags、ai_protocol、ai_socktype字段
  // 剩余的整数字段必须设置为0，指针字段必须为空
  memset(&hint, 0, sizeof(hint));
  hint.ai_socktype = SOCK_STREAM;
  hint.ai_canonname = NULL;
  hint.ai_addr = NULL;
  hint.ai_next = NULL;
  if ((err = getaddrinfo(argv[1], "ruptime", &hint, &ailist)) != 0)
    err_quit("getaddrinfo error: %s", gai_strerror(err));
  // 如果服务器支持多重网络接口或多重网络协议，函数getaddrinfo可能会返回多个候选地址供使用
  // 轮流尝试每个地址，当找到一个允许连接到服务的地址便可停止
  for (aip = ailist; aip != NULL; aip = aip->ai_next) {
    if ((sockfd = connect_retry(aip->ai_family, SOCK_STREAM, 0, aip->ai_addr,
                                aip->ai_addrlen)) < 0) {
      err = errno;
    } else {
      print_uptime(sockfd);
      exit(0);
    }
  }
  err_exit(err, "can't connect to %s", argv[1]);
}