#include "DataCorruptor.h"

// Attach to shared memory and retry if not found
int attach_shared_memory() {
    key_t key = ftok(".", SHM_KEY);
    if (key == -1) {
        log_event("Error generating shared memory key.");
        return -1;
    }

    int retries = 0;
    int shm_id;
    while ((shm_id = shmget(key, 0, 0)) == -1 && retries < MAX_RETRIES) {
        log_event("Shared memory not found. Retrying in 10 seconds...");
        sleep(10);
        retries++;
    }

    return (shm_id == -1) ? -1 : shm_id;
}

// Log events to dx.log
void log_event(const char *message) {
    FILE *log_file = fopen(LOG_FILE, "a");
    if (log_file) {
        time_t now = time(NULL);
        struct tm *local_time = localtime(&now);
        char time_str[20];
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", local_time);
        fprintf(log_file, "[%s] : %s\n", time_str, message);
        fclose(log_file);
    }
}

// Check if message queue exists
int check_message_queue() {
    int msg_queue_id = msgget(MSG_QUEUE_KEY, 0);
    return (msg_queue_id == -1) ? -1 : msg_queue_id;
}

// Kill a DC process
void kill_dc(int dc_id) {
    char log_msg[100];
    FILE *cmd;
    char pid_str[10];
    char command[50];

    snprintf(command, sizeof(command), "pgrep -f dc_%02d", dc_id);
    cmd = popen(command, "r");

    if (cmd) {
        if (fgets(pid_str, sizeof(pid_str), cmd) != NULL) {
            int pid = atoi(pid_str);
            if (kill(pid, SIGHUP) == 0) {
                snprintf(log_msg, sizeof(log_msg), "DX WOD rolled kill DC-%02d – Successfully killed process %d", dc_id, pid);
            } else {
                snprintf(log_msg, sizeof(log_msg), "DX WOD rolled kill DC-%02d – Failed to kill process %d", dc_id, pid);
            }
        } else {
            snprintf(log_msg, sizeof(log_msg), "DX WOD rolled kill DC-%02d – No such process found", dc_id);
        }
        pclose(cmd);
    } else {
        snprintf(log_msg, sizeof(log_msg), "DX WOD rolled kill DC-%02d – Error executing pgrep", dc_id);
    }
    log_event(log_msg);
}