#ifndef LOCKER_H
#define LOCKER_H
#include <stdint.h>
#include <stdlib.h>

struct locker_t {
  pid_t pid;
  uint16_t id;
  uint16_t user_id;
  uint8_t locked;
  uint8_t owned;
  int write_fd;
  int read_fd;
};

//You may use this, you don't need to though
struct queue_t {
  struct locker_t* head;
  size_t size;
};

#endif
