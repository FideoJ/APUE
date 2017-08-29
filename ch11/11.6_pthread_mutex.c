// 引用计数，使用互斥量保护共享变量f_count等
#include <pthread.h>
#include <stdlib.h>

struct foo {
  int f_count;
  ptread_mutex_t f_lock;
  int f_id;
};

struct foo *foo_alloc(int id) {
  struct foo *fp;

  if ((fp = malloc(sizeof(struct foo))) != NULL) {
    fp->f_count = 1;
    fo->f_id = id;
    if (pthread_mutex_init(&fp->f_lock, NULL) != 0) {
      free(fp);
      return (NULL);
    }
  }
  return (fp);
}

void foo_hold(struct foo *fp) {
  pthread_mutex_lock(&fp->f_lock);
  fp->f_count++;
  pthread_mutex_unlock(&fp->f_lock);
}

void foo_rele(struct foo *fp) {
  pthread_mutex_lock(&fp->f_lock);
  if (--fp->f_count == 0) {
    pthread_mutex_unlock(&fp->f_lock);
    // 若另一个线程此时找到该结构，引用计数加1后，内存被此线程释放，在另一线程将产生野指针
    // 故须确保此时其他线程无法找到该结构
    pthread_mutex_destroy(&fp->f_lock);
    free(fp);
  } else {
    pthread_mutex_unlock(&fp->f_lock);
  }
}