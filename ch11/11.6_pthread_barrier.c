// 子线程分段堆排，主线程归并。使用屏障等待所有子线程
// 可利用多处理器的优势
#include <apue.h>
#include <limits.h>
#include <pthread.h>
#include <sys/time.h>

#define NTHR 8
#define NUMNUM 8000000L
#define TNUM (NUMNUM / NTHR)

long nums[NUMNUM];
long snums[NUMNUM];

pthread_barrier_t b;

#ifdef SOLARIS
#define heapsort qsort
#else
extern int heapsort(void *, size_t, size_t,
                    int (*)(const void *, const void *));
#endif

int complong(const void *arg1, const void *arg2) {
  long l1 = *(long *)arg1;
  long l2 = *(long *)arg2;

  if (l1 == l2)
    return 0;
  else if (l1 < l2)
    return -1;
  else
    return 1;
}

void *thr_fn(void *arg) {
  long idx = (long)arg;

  heapsort(&nums[idx], TNUM, sizeof(long), complong);

  pid_t pid;
  pthread_t tid;
  pid = getpid();
  tid = pthread_self();
  printf("%s pid %lu tid %lu (0x%lx)\n", "Task completed: ", (unsigned long)pid,
         (unsigned long)tid, (unsigned long)tid);

  pthread_barrier_wait(&b);

  // Go off and perform more work ...
  return ((void *)0);
}

void merge() {
  long idx[NTHR];
  long i, minidx, sidx, num;

  for (i = 0; i < NTHR; i++)
    idx[i] = i * TNUM;
  // 逐一填充有序数组
  for (sidx = 0; sidx < NUMNUM; sidx++) {
    // 借助num找拥有当前最小元素的组号minidx
    num = LONG_MAX;
    for (i = 0; i < NTHR; i++) {
      // 可能整个组已读取完毕
      if ((idx[i] < (i + 1) * TNUM) && (nums[idx[i]] < num)) {
        num = nums[idx[i]];
        minidx = i;
      }
    }
    snums[sidx] = nums[idx[minidx]];
    // 去掉已读取的元素
    idx[minidx]++;
  }
}

int main() {
  unsigned long i;
  struct timeval start, end;
  long long startusec, endusec;
  double elapsed;
  int err;
  pthread_t tid;

  srandom(1);
  for (i = 0; i < NUMNUM; i++)
    nums[i] = random();

  gettimeofday(&start, NULL);
  pthread_barrier_init(&b, NULL, NTHR + 1);
  for (i = 0; i < NTHR; i++) {
    err = pthread_create(&tid, NULL, thr_fn, (void *)(i * TNUM));
    if (err != 0)
      err_exit(err, "can't create thread");
    printf("%s pid %lu tid %lu (0x%lx)\n", "Worker created: ",
           (unsigned long)getpid(), (unsigned long)tid, (unsigned long)tid);
  }
  pthread_barrier_wait(&b);
  merge();
  gettimeofday(&end, NULL);

  startusec = start.tv_sec * 1000000 + start.tv_usec;
  endusec = end.tv_sec * 1000000 + end.tv_usec;
  elapsed = (double)(endusec - startusec) / 1000000.0;
  printf("sort took %.4f seconds\n", elapsed);
  for (i = 0; i < NUMNUM; i++)
    printf("%ld\n", snums[i]);
  exit(0);
}