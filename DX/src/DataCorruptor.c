#include "DataCorruptor.h"

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
