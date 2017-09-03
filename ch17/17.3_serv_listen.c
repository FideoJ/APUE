#include <apue.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>

#define QLEN 10

int serv_listen(const char *name) {
  int fd, len, err, rval;
  struct sockaddr_un un;

  if (strlen(name) >= sizeof(un.sun_path)) {
    errno = ENAMETOOLONG;
    return (-1);
  }

  if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
    return (-2);

  unlink(name);

  memset(&un, 0, sizeof(un));
  un.sun_family = AF_UNIX;
  strcpy(un.sun_path, name);
  // 不需要设置某些平台提供的sun_len字段，因为操作系统会用传送给bind的地址长度设置该字段
  len = offsetof(struct sockaddr_un, sun_path) + strlen(name);

  if (bind(fd, (struct sockaddr *)&un, len) < 0) {
    rval = -3;
    goto errout;
  }

  if (listen(fd, QLEN) < 0) {
    rval = -4;
    goto errout;
  }
  return (fd);

errout:
  // 防止close可能出错而覆盖errno
  err = errno;
  close(fd);
  errno = err;
  return (rval);
}