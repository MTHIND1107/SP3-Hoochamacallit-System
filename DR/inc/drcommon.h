/**
 * @file drcommon.h
 * @brief Common definitions and functions for DR application
 */

 #ifndef DRCOMMON_H
 #define DRCOMMON_H
 
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <unistd.h>
 #include <signal.h>
 #include <time.h>
 #include <errno.h>
 #include <sys/types.h>
 #include <sys/ipc.h>
 #include <sys/msg.h>
 #include <sys/shm.h>
 
 /* Message structure */
 #define MSG_TYPE 1
 
 typedef struct {
     pid_t pid;
     int status;
 } MessageData;
 
 typedef struct {
     long mType;
     MessageData mData;
 } Message;
 
 /* Master list structure */
 #define MAX_DC_ROLES 10
 
 typedef struct {
     pid_t dcProcessID;
     time_t lastTimeHeardFrom;
 } DCInfo;
 
 typedef struct {
     int msgQueueID;
     int numberOfDCs;
     DCInfo dc[MAX_DC_ROLES];
 } MasterList;
 
 /* External variable declarations */
 extern int msgQueueId;
 extern int shmId;
 extern MasterList* masterList;
 extern FILE* logFile;
 
 /* Function prototypes */
 void cleanup(void);
 void log_message(const char* message);
 void signal_handler(int sig);
 int find_dc_by_pid(pid_t pid);
 int add_dc_to_master_list(pid_t pid, int status);
 void remove_dc_from_master_list(int index, const char* reason);
 void update_dc_in_master_list(int index, int status);
 void check_for_non_responsive_dcs(void);
 
 /* Logging function */
 static inline void log_message(const char* message) {
     if (logFile == NULL) {
         logFile = fopen("/tmp/dataMonitor.log", "a");
         if (logFile == NULL) {
             perror("Failed to open log file");
             return;
         }
     }
     
     time_t rawtime;
     struct tm* timeinfo;
     char timestamp[26];
     
     time(&rawtime);
     timeinfo = localtime(&rawtime);
     strftime(timestamp, sizeof(timestamp), "[%Y-%m-%d %H:%M:%S]", timeinfo);
     
     fprintf(logFile, "%s : %s\n", timestamp, message);
     fflush(logFile);
 }
 
 /* Get status description */
 static inline const char* get_status_description(int status) {
     switch (status) {
         case 0: return "Everything is OKAY";
         case 1: return "Hydraulic Pressure Failure";
         case 2: return "Safety Button Failure";
         case 3: return "No Raw Material in the Process";
         case 4: return "Operating Temperature Out of Range";
         case 5: return "Operator Error";
         case 6: return "Machine is Off-line";
         default: return "Unknown Status";
     }
 }
 
 #endif /* DRCOMMON_H */