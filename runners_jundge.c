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

mqd_t* queues_for_runners(size_t N, int msgflg, struct mq_attr* mq_attr) {
  char name[32] = 0;
  mqd_t* queue_array = (mqd_t*)calloc(sizeof(mqd_t*), N);
  for (size_t i = 0; i < N+1; i++)
  {
    snprintf(name, 32, "runner_%d", i+1);
    mqd_t queue_id = mq_open(name, msgflg, 0644, mq_attr);
    if(queue_id == (mqd_t)-1) {
      fprintf(stderr, "Failed to create queue: %s\n", 
              strerror(errno));
      exit(FAILURE_STATUS); 
    }
    
    queue_array[i] = queue_id;

    for(int j = 0; j < 32; j++) name[j] = '\0';
  }

  return queue_array; 
}

void queues_for_runners_delete(size_t N, mqd_t* queue_array) {
  char name[32] = 0;
  for (size_t i = 0; i < N+1; i++)
  {
    snprintf(name, 32, "runner_%d", i+1);
    destruct_queue(name, queue_array[i]);
    for(int j = 0; j < 32; j++) name[j] = '\0';
  }

  free(queue_array); queue_array = NULL;
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
  else  {
    printf("everything is OK\n");
  }
}

int judge(mqd_t queue_id, int N) {

  char buffer[MSG_SIZE];
  char message[16];
  unsigned int msg_prio;

  for (int i = 0; i < N; i++) {
    if (mq_receive(queue_id, buffer, sizeof(buffer), &msg_prio)==-1) {
      fprintf(stderr, "Failed to receive message from runner: %s\n", 
              strerror(errno));
      exit(FAILURE_STATUS);
    }

    //snprintf(message, 16, "%d", );
    //if (strncmp(buffer, 16, ))
    printf("Judge: runner %s is ready!\n", buffer);
    for(long j = 0; j < MSG_SIZE+1; j++) buffer[j] = '\0';
  }

  printf("Judge: Great! Everyone is ready! Start!\n");

  snprintf(message, 16, "-1");
  printf("buffer = %s\n", message);
  if (mq_send(queue_id, message, sizeof(message), 1) == -1) { //(char*)&msg - pointer to a struct
    perror("mq_send");
  }

  printf("sent\n");

  int status;
  for (long i = 0; i < N; i++)
    wait(&status);
  // msgsnd(queue_id, &buf, sizeof(int), 0);
  
  // if (msgrcv(queue_id, &buf, sizeof(int), N+1, 0) == -1) {
  //   fprintf(stderr, "Failed to receive message from runner: %s\n", strerror(errno));
  //   exit(FAILURE_STATUS);
  // }

  // printf("Judge: race is over!\n");
  return 0;
}

int runner(int runner_n, mqd_t queue_id, int N) {
  
  char message[16];
  unsigned int msg_prio;
  
  int length = snprintf(message, sizeof(message), "%d", runner_n);
  if (mq_send(queue_id, message, length, 1) == -1) {
    perror("mq_send");
  }

  sleep(1);

  while (1)
  {
    if (mq_receive(queue_id, message, sizeof(message), &msg_prio) != -1) {
      printf("Runner %d: Got message %s\n", runner_n, message);
      if (strncmp(message, "-1", 16) == 0) {
        printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
        // printf("Runner %d: giving estapheta to the next runner!\n", runner_n);
        // int got = atoi(message);
        // printf("runner got message: %d\n", got);
        // snprintf(message, 16, "%d", got - 1);
      }

      printf("Runner %d: return message = %s\n", runner_n, message);
      if (mq_send(queue_id, message, sizeof(message), 1) == -1) { //(char*)&msg - pointer to a struct
        perror("mq_send");
      }
    }
  }
    
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
  printf("exit\n");
  exit(SUCCESS_STATUS);
}

void init_runners(mqd_t* queue_array, int N) {
  for (int i = 0; i < N; i++) {
    pid_t pid = fork();

    if (pid == -1) {
      fprintf(stderr, "fork: %s\n", strerror(errno));
      exit(1);
    }

    if (pid == 0) {
      runner(i+1, queue_array, N);
      return;
    }
  } 
  
  return;
}
