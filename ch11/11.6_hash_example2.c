// hashlock保护散列表fh、散列链字段f_next、引用计数f_count
// f_lock保护foo中其他普通字段的访问
// 排序问题不再存在
// 若锁的粒度太粗，就会出现很多线程阻塞等待相同的锁，并发性下降。
// 若锁的粒度太细，则过多的锁开销会使系统性能受到影响，代码也变得复杂。
// 需在满足锁需求的情况下，在代码复杂性和性能之间找到平衡
#include <pthread.h>
#include <stdlib.h>

#define NHASH 29
#define HASH(id) (((unsinged long)id) % NHASH)

struct foo *fh[NHASH];

pthread_mutex_t hashlock = PTHREAD_MUTEX_INITIALIZER;

struct foo {
  int f_count;
  pthread_mutex_t f_lock;
  int f_id;
  struct foo *f_next; /* protected by hashlock */
  /* ... more stuff here ... */
};

struct foo *foo_alloc(int id) {
  struct foo *fp;
  int idx;

  if ((fp = malloc(sizeof(struct foo))) != NULL) {
    fp->f_count = 1;
    fp->f_id = id;
    if (pthread_mutex_init(&fp->f_lock, NULL) != 0) {
      free(fp);
      return (NULL);
    }
    idx = HASH(id);
    pthread_mutex_lock(&hashlock);
    fp->f_next = fh[idx];
    fh[idx] = fp;
    pthread_mutex_lock(&fp->f_lock);
    pthread_mutex_unlock(&hashlock);
    /* continue initialization */
    pthread_mutex_unlock(&fp->f_lock);
  }
  return (fp);
}

void foo_hold(struct foo *fp) {
  pthread_mutex_lock(&fp->hashlock);
  fp->f_count++;
  pthread_mutex_unlock(&fp->hashlock);
}

struct foo *foo_find(int id) {
  struct foo *fp;

  pthread_mutex_lock(&hashlock);
  for (fp = fh[HASH(id)]; fp != NULL; fp = fp->f_next) {
    if (fp->f_id == id) {
      fp->f_count++;
      break;
    }
  }
  pthread_mutex_unlock(&hashlock);
  return (fp);
}

void foo_rele(struct foo *fp) {
  struct foo *tfp;
  int idx;

  pthread_mutex_lock(&fp->hashlock);
  if (--fp->f_count == 0) {
    idx = HASH(fp->f_id);
    tfp = fh[idx];
    if (tfp == fp) {
      fh[idx] = fp->f_next;
    } else {
      while (tfp->f_next != fp)
        tfp = tfp->f_next;
      tfp->f_next = fp->f_next;
    }
    pthread_mutex_unlock(&hashlock);
    pthread_mutex_destroy(&fp->f_lock);
    free(fp);
  } else {
    pthread_mutex_unlock(&hashlock);
  }
}