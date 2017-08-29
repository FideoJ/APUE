// 使用pthread_cleanup_push注册线程清理处理程序：
// - 调用pthread_exit时；
// - 响应取消请求时；
// - 用非零execute参数调用pthread_cleanup_pop时。
// 若线程是通过从它的启动例程中返回而终止，不会调用清理处理程序
#include <apue.h>
#include <pthread.h>

void cleanup(void *arg) { printf("cleanup: %s\n", (char *)arg); }

void *thr_fn1(void *arg) {
  printf("thread 1 start\n");
  pthread_cleanup_push(cleanup, "thread 1 first handler");
  pthread_cleanup_push(cleanup, "thread 1 second handler");
  printf("thread 1 push complete\n");
  // 在调用pthread_cleanup_push和pthread_cleanup_pop之间返回，程序产生未定义行为
  if (arg)
    return ((void *)1);
  // pthread_cleanup_push可能被实现为宏，必须用pthread_cleanup_pop匹配，否则编译可能通不过
  pthread_cleanup_pop(0);
  pthread_cleanup_pop(0);
  return ((void *)1);
}

void *thr_fn2(void *arg) {
  printf("thread 2 start\n");
  pthread_cleanup_push(cleanup, "thread 2 first handler");
  pthread_cleanup_push(cleanup, "thread 2 second handler");
  printf("thread 2 push complete\n");
  if (arg)
    pthread_exit((void *)2);
  pthread_cleanup_pop(0);
  pthread_cleanup_pop(0);
  pthread_exit((void *)2);
}

int main() {
  int err;
  pthread_t tid1, tid2;
  void *tret;

  err = pthread_create(&tid1, NULL, thr_fn1, (void *)1);
  if (err != 0)
    err_exit(err, "can't create thread 1");
  err = pthread_create(&tid2, NULL, thr_fn2, (void *)1);
  if (err != 0)
    err_exit(err, "can't create thread 2");
  err = pthread_join(tid1, &tret);
  if (err != 0)
    err_exit(err, "can't join with thread 1");
  printf("thread 1 exit code %ld\n", (long)tret);
  err = pthread_join(tid2, &tret);
  if (err != 0)
    err_exit(err, "can't join with thread 2");
  printf("thread 2 exit code %ld\n", (long)tret);
  exit(0);
}