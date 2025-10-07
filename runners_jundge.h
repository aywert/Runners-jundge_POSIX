#ifndef RUNNERS
#define RUNNERS

#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/msg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>

enum message_type {
  NO_MSG = 0,
  START_MSG = -1, 
  READY_MSG = -2,
};

struct msgbuf {
  long mtype;       /* message type, must be > 0 */
  int  msg;    /* message data */
};

#define FAILURE_STATUS -1
#define SUCCESS_STATUS 1

static const long MSG_MAX = 10;
static const long MSG_SIZE = 1024;

int runner(int runner_n, mqd_t queue_id, int N);
int judge(mqd_t queue_id, int N);
mqd_t create_queue(const char *name, int msgflg, struct mq_attr* mq_attr);
void init_runners(mqd_t queue_id, int N);
void destruct_queue(const char* queue_name, mqd_t queue_id); 

#endif