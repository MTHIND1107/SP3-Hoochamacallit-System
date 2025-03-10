#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>

#ifndef DATACREATOR_H
#define DATACREATOR_H

int msgQueueExists(void);
void successMessage(int msgQueueId, pid_t pid);
void log_message(pid_t pid, int status);

#endif