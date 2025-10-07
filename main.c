#include "runners_jundge.h"
#include <sys/types.h>


static size_t N = 10;
const char* name = "/test_queue";

int main(void) {
  
  // char** msg_array = (char**)calloc( N+1, sizeof(char*));
  // for (size_t index; index < N+1; index++)
  // {
  //   msg_array[index] = (char*)calloc(MSG_SIZE, sizeof(char));
  //   msg_array[index] = itoa(index);
  // }
  struct mq_attr attr = {
    .mq_flags = 0,
    .mq_maxmsg = MSG_MAX,      // макс. сообщений в очереди
    .mq_msgsize = MSG_SIZE,   // макс. размер одного сообщения
    .mq_curmsgs = 0
  };

  mqd_t queue_id = create_queue(name, O_CREAT|O_RDWR, &attr);
  
  init_runners(queue_id, N);
  judge(queue_id, N);
  destruct_queue(name, queue_id);
  
  //init_runners(queue_id, N);  //starts N processes
  //judge(queue_id, N);         //gives start to the first runner
  //msgctl(queue_id, IPC_RMID, &My_st);

  return 0;
}