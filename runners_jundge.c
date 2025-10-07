#include "runners_jundge.h"   
                                                        
mqd_t create_queue(const char *name, int msgflg, struct mq_attr* mq_attr) {
  mqd_t queue_id = mq_open(name, msgflg, 0644, mq_attr);
  if(queue_id == (mqd_t)-1) {
    fprintf(stderr, "Failed to create queue: %s\n", 
            strerror(errno));
    printf("i am deadhere\n");
    exit(FAILURE_STATUS); 
  }

  return queue_id; 
}

void destruct_queue(const char* queue_name, mqd_t queue_id) {
  if (queue_id == (mqd_t)-1) {
    fprintf(stderr, "invalid file descriptor\n");
  }
  else if(mq_close(queue_id) == -1) {
    fprintf(stderr, "Failed to destruct queue: %s\n", 
            strerror(errno));
  }

  if(mq_unlink(queue_name) == -1) {
    if (errno != ENOENT) {
      fprintf(stderr, "Failed to destruct queue: %s\n", 
              strerror(errno));
      exit(FAILURE_STATUS); 
    }
    else {
      fprintf(stderr, "Queue is already destructed: %s\n", 
              strerror(errno));
    }
  }
  printf("everything is OK\n");
}

int judge(mqd_t queue_id, int N) {

  char buffer[MSG_SIZE+1];
  unsigned int msg_prio;

  for (int i = 0; i < N; i++) {
    if (mq_receive(queue_id, buffer, sizeof(buffer), &msg_prio)==-1) {
      fprintf(stderr, "Failed to receive message from runner: %s\n", 
              strerror(errno));
      exit(FAILURE_STATUS);
    }
    printf("Judge: runner %s is ready!\n", buffer);
    size_t j = 0; while(buffer[j] != '\0') buffer[j] = '\0';
  }

  printf("Judge: Great! Everyone is ready! Start!\n");

  int length = snprintf(buffer, MSG_SIZE, "start");
  if (mq_send(queue_id, buffer, length, 1) == -1) {
    perror("mq_send");
  }
  // msgsnd(queue_id, &buf, sizeof(int), 0);
  
  // if (msgrcv(queue_id, &buf, sizeof(int), N+1, 0) == -1) {
  //   fprintf(stderr, "Failed to receive message from runner: %s\n", strerror(errno));
  //   exit(FAILURE_STATUS);
  // }

  // printf("Judge: race is over!\n");
  return 0;
}

int runner(int runner_n, mqd_t queue_id, int N) {
  
  char message[10];
  int length = snprintf(message, 10, "%d", runner_n);

  if (mq_send(queue_id, message, length, 1) == -1) {
    perror("mq_send");
  }


  // if (msgrcv(queue_id, &buf, sizeof(int), runner_n, 0) == -1) {
  //   fprintf(stderr, "msgrcv1: %s\n", strerror(errno));
  //   exit(-1); 
  // }

  // if (runner_n < N) {
  //   buf.mtype = runner_n+1;
  //   buf.msg = START_MSG;
  //   for (int i = 0; i < N; i++) printf("runner_n = %d\n", runner_n);
  //   printf("runner %d: giving estapheta to next runner!\n", runner_n);

  //   if (msgsnd(queue_id, &buf, sizeof(int), 0) == -1) {
  //     fprintf(stderr, "Failed to send message: %s\n", strerror(errno));
  //     exit(FAILURE_STATUS); 
  //   }
  // }

  // else {
  //   buf.mtype = N+1;
  //   buf.msg = START_MSG;
  //   printf("runner %d: Hey jundge, my team is finished!\n", runner_n);

  //   if (msgsnd(queue_id, &buf, sizeof(int), 0) == -1) {
  //     fprintf(stderr, "Failed to send message: %s\n", strerror(errno));
  //     exit(FAILURE_STATUS); 
  //   }
  // }

  exit(SUCCESS_STATUS);
}

void init_runners(mqd_t queue_id, int N) {
  for (int i = 0; i < N; i++) {
    pid_t pid = fork();

    if (pid == -1) {
      fprintf(stderr, "fork: %s\n", strerror(errno));
      exit(1);
    }

    if (pid == 0) {
      runner(i+1, queue_id, N);
      return;
    }
  } 
  
  return;
}
