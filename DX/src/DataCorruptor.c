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
                snprintf(log_msg, sizeof(log_msg), "DX WOD rolled kill DC-%02d Successfully killed process %d", dc_id, pid);
            } else {
                snprintf(log_msg, sizeof(log_msg), "DX WOD rolled kill DC-%02d Failed to kill process %d", dc_id, pid);
            }
        } else {
            snprintf(log_msg, sizeof(log_msg), "DX WOD rolled kill DC-%02d No such process found", dc_id);
        }
        pclose(cmd);
    } else {
        snprintf(log_msg, sizeof(log_msg), "DX WOD rolled kill DC-%02d Error executing pgrep", dc_id);
    }
    log_event(log_msg);
}

// Delete the message queue
void delete_message_queue() {
    int msg_queue_id = msgget(MSG_QUEUE_KEY, 0);
    if (msg_queue_id != -1) {
        if (msgctl(msg_queue_id, IPC_RMID, NULL) == 0) {
            log_event("Successfully deleted message queue");
        } else {
            log_event("Failed to delete message queue");
        }
    } else {
        log_event(" Message queue not found");
    }
}

// Execute the action from the Wheel of Destruction
void execute_action(int action) {
    char log_msg[100];

    switch (action) {
        case 0: case 8: case 19:
            snprintf(log_msg, sizeof(log_msg), "DX WOD rolled %02d – Doing nothing", action);
            log_event(log_msg);
            break;
        case 1: case 4: case 11:
            kill_dc(1);
            break;
        case 3: case 6: case 13:
            kill_dc(2);
            break;
        case 2: case 5: case 15:
            kill_dc(3);
            break;
        case 7:
            kill_dc(4);
            break;
        case 9:
            kill_dc(5);
            break;
        case 12:
            kill_dc(6);
            break;
        case 14:
            kill_dc(7);
            break;
        case 16:
            kill_dc(8);
            break;
        case 18:
            kill_dc(9);
            break;
        case 20:
            kill_dc(10);
            break;
        case 10: case 17:
            delete_message_queue();
            break;
        default:
            snprintf(log_msg, sizeof(log_msg), "DX WOD rolled an invalid number %02d", action);
            log_event(log_msg);
    }
}

//Main function
int main() {
    srand(time(NULL));
    
    int shm_id = attach_shared_memory();
    if (shm_id == -1) {
        log_event("DX failed to attach to shared memory after max retries. Exiting.");
        return 1;
    }

    while (1) {
        // Step 1: Sleep for a random duration between 10-30 seconds
        sleep(10 + rand() % 21);

        // Step 2: Check if the message queue still exists
        if (check_message_queue() == -1) {
            log_event("DX detected that msgQ is gone assuming DR/DCs done");
            break;
        }

        // Step 3: Pick a random action from "Wheel of Destruction"
        int action = rand() % 21;

        // Step 4: Execute the selected action
        execute_action(action);
    }

    return 0;
}
