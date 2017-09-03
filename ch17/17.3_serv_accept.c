//
#include <apue.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <time.h>

#define STALE 30

int serv_accept(int listenfd, uid_t *uidptr) {
  int clifd, err, rval;
  socklen_t len;
  time_t staletime;
  struct sockaddr_un un;
  struct stat statbuf;
  char *name;

  if ((name = malloc(sizeof(un.sun_path) + 1) == NULL))
    return (-1);
  len = sizeof(un);
  // un拿到客户进程套接字路径名，len拿到实际的长度
  if ((clifd = accept(listenfd, (struct sockaddr *)&un, &len)) < 0) {
    free(name);
    return (-2);
  }

  len -= offsetof(struct sockaddr_un, sun_path);
  memcpy(name, un.sun_path, len);
  // 确保以null终止
  name[len] = 0;
  if (stat(name, &statbuf) < 0) {
    rval = -3;
    goto errout;
  }
// 通过以下检验，可认为客户进程的身份（其有效用户id）是该套接字所有者
// 虽然这种检验并不完善，但这是对当前系统所能做到的最佳方案
#ifdef S_ISSOCK
  // 确实是一个套接字
  if (S_ISSOCK(statbuf.st_mode) == 0) {
    rval = -4;
    goto errout;
  }
#endif
  // 其权限仅允许用户读、用户写、用户执行（700）
  if ((statbuf.st_mode & (S_IRWXG | S_IRWXO)) ||
      (statbuf.st_mode & S_IRWXU) != S_IRWXU) {
    rval = -5;
    goto errout;
  }

  staletime = time(NULL) - STALE;
  // 三个时间参数不比当前时间早30秒
  if (statbuf.st_atime < staletime || statbuf.st_ctime < staletime ||
      statbuf.st_mtime < staletime) {
    rval = -6;
    goto errout;
  }

  if (uidptr != NULL)
    *uidptr = statbuf.st_uid;
  unlink(name);
  free(name);
  return (clifd);

errout:
  err = errno;
  close(clifd);
  free(name);
  errno = err;
  return (rval);
}