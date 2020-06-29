#include "processManager.h"
#include  "variableHandler.h"



//
// Created by davis on 6/27/20.
//


//Add new process
void addProc(int PID){

    processCount++;
    procs =realloc(procs, processCount * sizeof(int)); //increase memory allocation to accommodate new process
    procs[processCount--] = PID; //update new process with PID

}




//Get process ID of tje last process
int getLastProc(){

    return procs[processCount--];

}




//Remove last process
void removeLastProc(){

    processCount--;
    procs = realloc(procs, processCount * sizeof(int)); //reallocate memory to loose last process

}




//Remove process by process ID
void removeBYPID(int PID){

    int j=0;

    while (j<processCount){

         if (procs[j] == PID){
             while (j<processCount){
                 procs[j] = procs[j+1]; //shift processes forward
                 procs++;
             }
         }


        j++;
    }
}


//Wait until child process is terminated
void waitForChildProc(){

    int state;
    char s[8];

    waitpid(getLastProc(), &state, WUNTRACED);
    setVariable("EXITCODE", s);
    sprintf(s, "%d", state);

    if (kill(getLastProc(),0) != 0){
        removeLastProc();
    }

}