#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

#define MSG_QUEUE_KEY 1234
#define LOG_FILE "dc_log.txt"
#define MAX_MACHINES 10

// Message structure
struct message {
    long msg_type;
    int machine_id;
    int status;
};

// Status Codes
enum StatusCodes {
    OKAY = 0,
    HYDRAULIC_FAILURE,
    SAFETY_FAILURE,
    NO_RAW_MATERIAL,
    TEMP_OUT_OF_RANGE,
    OPERATOR_ERROR,
    OFFLINE
};

const char *status_messages[] = {
    "Everything is OKAY",
    "Hydraulic Pressure Failure",
    "Safety Button Failure",
    "No Raw Material in the Process",
    "Operating Temperature Out of Range",
    "Operator Error",
    "Machine is Off-line"
};