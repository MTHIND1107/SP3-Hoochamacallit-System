#ifndef DATA_CORRUPTOR_H
#define DATA_CORRUPTOR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>

// Constants
#define SHM_KEY 16535       // Shared Memory Key for DX
#define MSG_QUEUE_KEY 1234  // Message Queue Key for DCs & DR
#define MAX_RETRIES 100     // Max retries for shared memory
#define LOG_FILE dc.log

// Function Declarations
int attach_shared_memory();
void log_event(const char *message);
void execute_action(int action);
int check_message_queue();
void kill_dc(int dc_id);
void delete_message_queue();

#endif
