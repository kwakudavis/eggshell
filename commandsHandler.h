#include <sys/wait.h>
#include <stdbool.h>

//
// Created by davis on 6/27/20.
//

//Executes echo command
void echoCommand(char** Args);

//Execute change directory command
void cdCommand(char* DIR);


//Update content of variable
void updateVarCommand(char* lineString);

//Forks child
void forkChildCommand(char** Args, int* pipeInput, int* pipeOutput, bool readPipe, bool writePipe);


//Executes source command
void sourceCommand(char* filename, void recursiveCallbackFunc(char* lineString));

//Print all shell and enviroment variables
void showenvCommand();


//executes the unset command
void unsetCommand(char* variableName);






#ifndef EGGSHELL_COMMANDSHANDLER_H
#define EGGSHELL_COMMANDSHANDLER_H

#endif //EGGSHELL_COMMANDSHANDLER_H
