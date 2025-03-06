#include "DataCorruptor.h"

// Attach to shared memory and retry if not found
int attach_shared_memory() {
    key_t key = ftok(".", SHM_KEY);
    if (key == -1) {
        log_event("Error generating shared memory key.");
        return -1;
    }