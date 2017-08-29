// 条件变量符合“等待-唤醒”的场景，避免了频繁进入临界区加锁解锁造成的资源浪费
// 条件本身由互斥量保护
// 这就关闭了条件检查和线程进入休眠状态等待条件改变这两个操作之间的时间通道，这样线程就不会错过条件的任何变化
// pthread_cond_wait先让线程阻塞，然后对传入的互斥量解锁（以使生产者能改变条件并通知），调用返回时再次加锁互斥量
#include <pthread.h>

struct msg {
  struct msg *m_next;
  /* ... more stuff here ... */
};

struct msg *workq;

pthread_cond_t qready = PTHREAD_COND_INITIALIZER;

pthread_mutex_t qlock = PTHREAD_MUTEX_INITIALIZER;

void process_msg() {
  struct msg *mp;

  for (;;) {
    pthread_mutex_lock(&qlock);
    // 可能先被其它线程抢到条件，以及防止虚假通知
    while (workq == NULL)
      pthread_cond_wait(&qready, &qlock);
    mp = workq;
    workq = mp->m_next;
    pthread_mutex_unlock(&qlock);
    /* now process the message mp */
  }
}

void enqueue_msg(struct msg *mp) {
  pthread_mutex_lock(&qlock);
  mp->m_next = workq;
  workq = mp;
  pthread_mutex_unlock(&qlock);
  // 先解除对互斥量的占有，再通知正在等待的线程
  pthread_cond_signal(&qready);
}