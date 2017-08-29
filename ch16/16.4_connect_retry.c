#include <apue.h>
#include <sys/socket.h>

#define MAXSLEEP 128

// 不可移植，若connect失败，套接字状态为未定义
int connect_retry(int sockfd, const struct sockaddr *addr, socklen_t alen) {
  int numsec;

  for (numsec = 1; numsec <= MAXSLEEP; numsec <<= 1) {
    if (connect(sockfd, addr, alen) == 0) {
      return (0);
    }

    if (numsec <= MAXSLEEP / 2)
      sleep(numsec);
  }
  return (-1);
}

int connect_retry(int domain, int type, int protocol,
                  const struct sockaddr *addr, socklen_t alen) {
  int numsec, fd;

  for (numsec = 1; numsec <= MAXSLEEP; numsec <<= 1) {
    if ((fd = socket(domain, type, protocol)) < 0)
      return (-1);

    if (connect(fd, addr, alen) == 0) {
      return fd;
    }
    close(fd);

    if (numsec <= MAXSLEEP / 2)
      sleep(numsec);
  }
  return (-1);
}