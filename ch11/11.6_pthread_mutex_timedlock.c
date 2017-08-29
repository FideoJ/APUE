// pthread_mutex_timedlock可进行可能超时的上锁，使用的是绝对时间
#include <apue.h>
#include <pthread.h>

int main() {
  int err;
  struct timespec tout;
  struct tm *tmp;
  char buf[64];
  // 静态分配的互斥量可这样初始化，且不需pthread_mutex_destroy
  pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

  pthread_mutex_lock(&lock);
  printf("mutex is locked\n");
  clock_gettime(CLOCK_REALTIME, &tout);
  tmp = localtime(&tout.tv_sec);
  strftime(buf, sizeof(buf), "%r", tmp);
  printf("current time is %s\n", buf);
  tout.tv_sec += 10;
  // 两次加锁一般会导致死锁
  err = pthread_mutex_timedlock(&lock, &tout);
  clock_gettime(CLOCK_REALTIME, &tout);
  tmp = localtime(&tout.tv_sec);
  strftime(buf, sizeof(buf), "%r", tmp);
  printf(" the time is now %s\n", buf);
  if (err == 0)
    printf("mutex locked again!\n");
  else
    printf("can't lock mutex again:%s\n", strerror(err));
  exit(0);
}