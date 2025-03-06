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