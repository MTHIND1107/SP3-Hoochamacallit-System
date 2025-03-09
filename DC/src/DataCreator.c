//Wake up pick a random status and keep on sending until DC picks no. 7, and it never expects a response.
//Generate a log file, and write messages in the log file.
//Expectations of the logging
//Feel free print whatever we need to the screen but at the end the DC should print nothing.
//only log messages allowed no other ouput to the screen.
//10 DC should be running
//DC needs to know the way queue is being used in DR
#include "DataCreator.h"

#define MSG_QUEUE_KEY 1234
#define LOG_FILE "dc_log.txt"
#define MAX_MACHINES 10
#define PROJECT_ID 1

// Message structure
struct message {
    long msg_type;
    int machine_id;
    int status;
    char mtext[100];
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
    struct message messageBuffer;
    size_t bufferLength;
    pid_t pid = getpid();
    //Checking for the message queue
    int msgQueueId = msgQueueExists();
    //If queue id found, then the first message sent to the DR
    successMessage(msgQueueId, pid);
    //This is the main loop where the random messages are sent until the random number lands on Machine is off-line.
    srand(time(NULL));
    while (1) {
        // Generate random status
        int status = rand() % 7;

        // Prepare message
        snprintf(messageBuffer.mtext, sizeof(messageBuffer.mtext), "PID: %d, Status: %s", pid, status_messages[status]);
        bufferLength = strlen(messageBuffer.mtext) + 1;

        // Send message
        if (msgsnd(msgQueueId, &messageBuffer, bufferLength, IPC_NOWAIT) < 0) {
            perror("msgsnd");
            exit(1);
        }
        log_message(pid, status);

        // Exit if status is "Machine is Off-line"
        if (status == 6) {
            break;
        }

        // Sleep for a random time between 10 and 30 seconds
        sleep(10 + rand() % 21); //CHANGE THESE TO CONSTANTS
    }

    return 0;
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
/*
* Name: successMessage()
* Returns: nothing
* Parameters: int msgQueueId
* Description: Sends the initial message after the message queue is found.
*              And laso logs in the message for DC.
*/
void successMessage(int msgQueueId, pid_t pid){
    struct message messageBuffer;
    size_t bufferLength;
    messageBuffer.msg_type = 1; ;
    //messageBuffer contains the PID and the first message
    snprintf(messageBuffer.mtext, sizeof(messageBuffer.mtext), "PID: %d, %s", pid, status_messages[0]);
    bufferLength = strlen(messageBuffer.mtext) + 1;
    // Initial message sent to DR.
    if (msgsnd(msgQueueId, &messageBuffer, bufferLength, IPC_NOWAIT) < 0) {
        perror("msgsnd");
        exit(1);
    }
    log_message(pid,0); //Logs the message for DC PID and '0' for the everything OKAY message.
}
/*
* Name: log_message()
* Returns: nothing
* Parameters: pid_t pid, int status
* Description: For logging in the message in the log file in the temp folder.
*              And also implemeting the format provided for keeping in the logs.
*/
void log_message(pid_t pid, int status) {
    FILE *log_file = fopen("/tmp/dataCreator.log", "a"); //The log file position
    if (log_file != NULL) {
        time_t now = time(NULL);
        struct tm *local_time = localtime(&now);//The current time
        char time_str[20];
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", local_time); //The time format created according to the example shown
        //Prints the entire log format to the log file in the way asked for.
        //Example Format:
        //[2020-03-06 21:05:07] : DC [5687] – MSG SENT – Status 2 (Safety Button Failure)
        fprintf(log_file, "[%s] : DC [%d] - MSG SENT - Status %d (%s)\n", time_str, pid, status, status_messages[status]);
        fclose(log_file);
    }
}

