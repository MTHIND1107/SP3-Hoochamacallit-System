//Wake up pick a random status and keep on sending until DC picks no. 7, and it never expects a response.
//Generate a log file, and write messages in the log file.
//Expectations of the logging
//Feel free print whatever we need to the screen but at the end the DC should print nothing.
//only log messages allowed no other ouput to the screen.
//10 DC should be running
//DC needs to know the way queue is being used in DR
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MSG_QUEUE_KEY 1234
#define LOG_FILE "dc_log.txt"
#define MAX_MACHINES 10
#define PROJECT_ID 1

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
int main(void){
    
}


/*
* Name: msgQueueExists()
* Returns: int
* Parameters: None
* Description: Will keep on going in forever loop if never found the Message queue.
*              It basically only checks if the message queue exists or not.
*/
int msgQueueExists(void){
    int msgQueueId;
    key_t key;
    // Generate a unique key message key tusing ftok
    if ((key = ftok(".", PROJECT_ID)) == -1) { //-1 is used to represent if no key was produced
        perror("ftok");
        return -1;
    }
    //The while loop which checks if the message Queue exists or not
    //If not then DC goes to sleep for 10 sec and try again until a queue found
    //The 'key' used in here is from the ftok above.
    while ((msgQueueId = msgget(key, 0)) < 0) { 
        perror("msgget");
        printf("Message queue not found. Retrying in 10 seconds...\n");
        sleep(10); //Sleep in for 10 seconds
    }
    return msgQueueId;
}
