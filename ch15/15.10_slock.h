#ifndef SLOCK_H_
#define SLOCK_H_

#include <semaphore.h>

struct slock {
  sem_t *semp;
  char name[_POSIX_NAME_MAX];
};
#endif