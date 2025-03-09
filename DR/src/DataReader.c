 /* programmer name : krishna kachhiapatel 
 * main.c
 * @brData Reader (DR) application for the Hoochamacallit System
 */

#include "drcommon.h"

 /* Global variables */
 int msgQueueId = -1;
 int shmId = -1;
 MasterList* masterList = NULL;
 FILE* logFile = NULL;
 
 /* Signal handler */
 void signal_handler(int sig) {
     char buffer[128];
     snprintf(buffer, sizeof(buffer), "Received signal %d, cleaning up and exiting", sig);
     log_message(buffer);
     cleanup();
     exit(EXIT_SUCCESS);
 }
 
 /* Cleanup function */
 void cleanup(void) {
     log_message("DR cleanup initiated");
     
     if (msgQueueId != -1) {
         if (msgctl(msgQueueId, IPC_RMID, NULL) == -1) {
             perror("msgctl failed in cleanup");
         } else {
             log_message("Message queue removed successfully");
         }
     }
     
     if (masterList != NULL) {
         if (shmdt(masterList) == -1) {
             perror("shmdt failed in cleanup");
         }
     }
     
     if (shmId != -1) {
         if (shmctl(shmId, IPC_RMID, NULL) == -1) {
             perror("shmctl failed in cleanup");
         }
     }
     
     if (logFile != NULL) {
         fclose(logFile);
     }
     
     log_message("All DCs have gone offline or terminated - DR TERMINATING");
 }
 
 /* Find DC by PID */
 int find_dc_by_pid(pid_t pid) {
     for (int i = 0; i < masterList->numberOfDCs; i++) {
         if (masterList->dc[i].dcProcessID == pid) {
             return i;
         }
     }
     return -1;
 }
 
 /* Add DC to master list */
 int add_dc_to_master_list(pid_t pid, int status) {
     if (masterList->numberOfDCs >= MAX_DC_ROLES) {
         log_message("ERROR: Cannot add more DCs, master list is full");
         return -1;
     }
     
     int dc_idx = masterList->numberOfDCs;
     masterList->dc[dc_idx].dcProcessID = pid;
     masterList->dc[dc_idx].lastTimeHeardFrom = time(NULL);
     masterList->numberOfDCs++;
     
     char buffer[256];
     snprintf(buffer, sizeof(buffer), "DC-%02d [%d] added to the master list - NEW DC - Status 0 (Everything is OKAY)", 
              dc_idx + 1, pid);
     log_message(buffer);
     
     return 0;
 }
 
 /* Remove DC from master list */
 void remove_dc_from_master_list(int index, const char* reason) {
     if (index < 0 || index >= masterList->numberOfDCs) {
         return;
     }
     
     pid_t pid = masterList->dc[index].dcProcessID;
     
     char buffer[256];
     snprintf(buffer, sizeof(buffer), "DC-%02d [%d] %s from master list - %s", 
              index + 1, pid, 
              (strcmp(reason, "OFFLINE") == 0) ? "has gone OFFLINE - removing" : "removed",
              reason);
     log_message(buffer);
     
     /* Collapse the list */
     for (int i = index; i < masterList->numberOfDCs - 1; i++) {
         masterList->dc[i] = masterList->dc[i + 1];
     }
     
     masterList->numberOfDCs--;
 }
 
 /* Update DC in master list */
 void update_dc_in_master_list(int index, int status) {
     if (index < 0 || index >= masterList->numberOfDCs) {
         return;
     }
     
     masterList->dc[index].lastTimeHeardFrom = time(NULL);
     
     const char* status_desc = get_status_description(status);
     
     char buffer[256];
     snprintf(buffer, sizeof(buffer), "DC-%02d [%d] updated in the master list - MSG RECEIVED - Status %d (%s)", 
              index + 1, masterList->dc[index].dcProcessID, status, status_desc);
     log_message(buffer);
 }
 
 /* Check for non-responsive DCs */
 void check_for_non_responsive_dcs(void) {
     time_t current_time = time(NULL);
     
     for (int i = masterList->numberOfDCs - 1; i >= 0; i--) {
         time_t last_time = masterList->dc[i].lastTimeHeardFrom;
         
         if (difftime(current_time, last_time) > 35.0) {
             remove_dc_from_master_list(i, "NON-RESPONSIVE");
         }
     }
 }
 
 /* Main function */
 int main(void) {
     log_message("DR starting up");
     
     /* Set up signal handlers */
     signal(SIGINT, signal_handler);
     signal(SIGTERM, signal_handler);
     
     /* Register cleanup function */
     atexit(cleanup);
     
     /* Initialize message queue */
     key_t msgKey = ftok(".", 16534);
     if (msgKey == -1) {
         log_message("ftok failed for message queue");
         return EXIT_FAILURE;
     }
     
     msgQueueId = msgget(msgKey, IPC_CREAT | 0666);
     if (msgQueueId == -1) {
         log_message("msgget failed");
         return EXIT_FAILURE;
     }
     
     /* Initialize shared memory */
     key_t shmKey = ftok(".", 16535);
     if (shmKey == -1) {
         log_message("ftok failed for shared memory");
         return EXIT_FAILURE;
     }
     
     shmId = shmget(shmKey, sizeof(MasterList), IPC_CREAT | 0666);
     if (shmId == -1) {
         log_message("shmget failed");
         return EXIT_FAILURE;
     }
     
     masterList = (MasterList *)shmat(shmId, NULL, 0);
     if (masterList == (void *)-1) {
         log_message("shmat failed");
         return EXIT_FAILURE;
     }
     
     masterList->msgQueueID = msgQueueId;
     masterList->numberOfDCs = 0;
     
     log_message("DR initialized successfully, waiting 15 seconds for DCs to connect");
     
     /* Wait for 15 seconds to give time for DCs to start */
     sleep(15);
     
     log_message("DR entering main processing loop");
     
     /* Main processing loop */
     Message msg;
     ssize_t bytes_read;
     
     while (1) {
         /* Check for non-responsive DCs */
         check_for_non_responsive_dcs();
         
         /* Check if all DCs are gone */
         if (masterList->numberOfDCs == 0) {
             log_message("All DCs have gone offline or terminated - DR TERMINATING");
             break;
         }
         
         /* Try to receive a message (non-blocking) */
         bytes_read = msgrcv(msgQueueId, &msg, sizeof(msg.mData), 0, IPC_NOWAIT);
         
         if (bytes_read == -1) {
             if (errno != ENOMSG) {
                 perror("msgrcv failed");
                 break;
             }
         } else {
            pid_t sender_pid = msg.mData.machine_id;  // Or extract it from mtext if that's how it's stored
            int status = msg.mData.status;
             
             int dc_idx = find_dc_by_pid(sender_pid);
             
             if (dc_idx == -1) {
                 /* New DC */
                 add_dc_to_master_list(sender_pid, status);
             } else {
                 if (status == 6) {
                     /* Machine is off-line */
                     remove_dc_from_master_list(dc_idx, "OFFLINE");
                 } else {
                     /* Update DC entry */
                     update_dc_in_master_list(dc_idx, status);
                 }
             }
         }
         
         /* Sleep for 1.5 seconds before next iteration */
         usleep(1500000);
     }
     
     return EXIT_SUCCESS;
 }