#include <wait.h>
#include <stdlib.h>

int processCount;
int* procs;

//Add new process
void addProc(int PID);

//Get process ID of tje last process
int getLastProc();

//Remove last process
void removeLastProc();


//Remove process by process ID
void removeBYPID(int PID);

//Wait until child process is terminated
void waitForChildProc();