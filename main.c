#include "linenoise.h"
#include "commandsHandler.h"
#include "stringParser.h"
#include "processManager.h"


//parse input string to be executed
void parseLine(char* lineString);

//Parse string for output redirection
char* outputRedirectionParser(char * lineString);


//parse line for input redirection
char* inputReceiveParser(char* lineString);

//parse for piping
void pipingParser(char* lineString);


//Executes commands
void executeCommand(char* lineString);


//Initialize Shell variables
void initializeShellVariables(char* arg0);

//Initialize process and signals handling
void initializeSignalsProcessHandler();

//signal handling
void signalHandler(int signal);

//Resets command variables
void resetCommands();

//resets child process state variable
void IOreset();


//Signal handling data struct and standard input and output data struct
struct sigaction sig;

int stdinFileDesc;
int stdoutFileDesc;
int stderrFIleDesc;


//File redirection data structures
FILE* fileOutput;
FILE* fileInput;


//Pipe Data struct declaration
int pipeA[2];
int pipeB[2];

//child process state
bool intIO[2];


//Pipe type
bool currPipe = 0;


//parent process wait state

bool parentProcWait = true;



int main(int argc, char *argv[]) {
    //CLear IO
    IOreset();

    //Set linenoise max length
    linenoiseHistorySetMaxLen(16);

    char* linestring; //line from string from terminal



    //Create additional file descriptors
    stderrFIleDesc = dup(STDERR_FILENO);
    stdinFileDesc = dup(STDIN_FILENO);
    stdoutFileDesc =dup(STDOUT_FILENO);


    //Initialize shell variables and signal handling
    initializeSignalsProcessHandler();
   initializeShellVariables(argv[0]);




    while (1){


        //check that line is not NULL
        linestring = linenoise(getVariable("PROMPT"));
        if(linestring == NULL){continue;}

        char sline[variableSize];
        strncpy(sline, linestring, variableSize);

        stripSpaces(sline); //strip spaces off strings that contain spaces

        if (strlen(sline) <= 0 ){ continue;}

        linenoiseHistoryAdd(linestring);//update history


        //parse line string and release it to be reset
       parseLine(linestring);
       linenoiseFree(linestring);


    }




}



//Initialize Shell variables
void initializeShellVariables(char* arg0){
    variableCount = 0;
    variablesContainer = malloc(0);


        setVariable("PATH", "/usr/bin:/bin:/usr/local/bin");
      setVariable("PROMPT", "> ");
        setVariable("CWD", "");
       setVariable("USER", "usr");
      setVariable("HOME", "/home");
       setVariable("SHELL", "");
     setVariable("TERMINAL", "/usr/bin");
       setVariable("EXITCODE", "0");


    initializeVariables(arg0);

}



//Initialize process and signals handling
void initializeSignalsProcessHandler(){

    processCount = 0;

    procs = malloc(0);

    if (sigaction(SIGINT, &sig, NULL) == -1 ||sigaction(SIGTSTP, &sig, NULL) == -1 ||sigaction(SIGCHLD, &sig, NULL) == -1){
        fprintf(stderr, "ERROR: NO signals caught");
    }

}



//Parse string for output redirection
char* outputRedirectionParser(char * lineString){

    char* Args[2];

    parseStringByString(lineString, ">>", Args);

    //check if line string does not contain a NULL assignment
    if (Args[1] != NULL && Args[1][0] != '\0' ) {


        //Append output to file

        if((fileOutput = fopen(Args[1], "a"))){
            dup2(fileno(fileOutput), STDOUT_FILENO);
            dup2(fileno(fileOutput), STDERR_FILENO);

        }else{
            perror("Error:Unable to redirect output to file");
        }



    }else{ //Write instead of appending

        parseStringByString(lineString, ">", Args);
        if (Args[1] != NULL && Args[1][0] != '\0' ) {


            //Append output to file

            if ((fileOutput = fopen(Args[1], "w"))) {
                dup2(fileno(fileOutput), STDOUT_FILENO);
                dup2(fileno(fileOutput), STDERR_FILENO);

            } else {
                perror("Error:Unable to redirect output to file");
            }
        }
    }
        return  Args[0];

}




//parse line for input redirection
char* inputReceiveParser(char* lineString){

    char* Args[2];

    parseStringByString(lineString, "<<<", Args);

    if ( Args[1] != NULL && Args[1][0] != '\0' ){
        fileInput = tmpfile();
        fprintf(fileInput, "%s", Args[1]);
        rewind(fileInput);
        dup2(fileno(fileInput), STDIN_FILENO);
    } else{

        parseStringByChar(lineString, "<", Args);

        if (Args[1] != NULL){
            //Format string and read input
            stripSpaces(Args[1]);
            if ((fileInput = fopen(Args[1], "r"))){
                dup2(fileno(fileInput), STDIN_FILENO);
            }
        }

    return Args[0];
    }


}


//reset child process state
void IOreset(){
    intIO[0]= 0;
    intIO[1]=0;
}



//Signal Handling
void signalHandler(int signal){
    int state;


     //Handling external child process interrupts
    if (signal == SIGCHLD){
        int s = waitpid(-1, &state, WNOHANG);
        if (s>0 && kill(s,0) != 0){
            printf("Child Process Aborted\n");
            removeBYPID(s); //remove process from stack
        }
    }

    if (signal == SIGINT){
        printf("Process Aborted\n");
        kill(getLastProc(),SIGKILL);
    }
    else if(signal == SIGTSTP){
        printf("Process Ended\n");
        kill(getLastProc(),SIGSTOP);
    }

}



//Executes commands
void executeCommand(char* lineString) {

    char *Args[MAX_ARGS];

    parseStringByChar(lineString, " ", Args); //Parse line string content


    //Execute a command function depending on the content of the string

    if (strcmp(Args[0], "exit") == 0) { //Internal commands
        exit(0);
    } else if (strcmp(Args[0], "source") == 0) {
        sourceCommand(Args[1], &parseLine);


    } else if (strcmp(Args[0], "cd") == 0) {
        cdCommand(Args[1]);

     }else if(strcmp(Args[0], "unset") == 0){

            unsetCommand(Args[1]);
}else if (strcmp(Args[0], "echo") == 0) {

        echoCommand(Args);

    } else if (strcmp(Args[0], "showenv") == 0) {
        showenvCommand();
    } else if (strcmp(Args[0], "bg") == 0) {

        if (processCount > 0) {
            kill(getLastProc(), SIGCONT);
        } else {
            fprintf(stderr, "Error: No current process running\n");
        }
    } else if (strcmp(Args[0], "fg") == 0) {

        if (processCount > 0) {
            kill(getLastProc(), SIGCONT);
            pause();
            waitForChildProc();
        } else {
            fprintf(stderr, "Error: No current process running\n");
        }

    } else if (Args[0][0] == '$') {

        char updateVariableString[variableSize];
        strncpy(updateVariableString, Args[0], variableSize);

        for (int j = 1; Args[j] != NULL; j++) {
            strncat(updateVariableString, Args[j], variableSize);
        }

        updateVarCommand(updateVariableString);

    } else {//external commands

        if (currPipe) {
            forkChildCommand(Args, pipeA, pipeB, intIO[0], intIO[1]);
        } else {
            forkChildCommand(Args, pipeB, pipeA, intIO[0], intIO[1]);

        }

        currPipe = !currPipe;

    }
}




//Resets command variables for file redirection
 void resetCommands(){



    dup2(stdinFileDesc, 0);
    dup2(stdoutFileDesc, 1);
    dup2(stderrFIleDesc, 2);

    //CLose file active file streams
    if (fileInput != NULL){
            fclose(fileInput);
            fileInput = NULL;
    }

    if (fileOutput != NULL){
            fclose(fileOutput);
            fileOutput = NULL;
    }
    IOreset();
    currPipe = 0;



}


//parse input string for piping
void pipingParser(char* lineString){
    char * Args[MAX_ARGS];
    int no_of_arguments = 0;


    //Parse string by parsing character |

    parseStringByChar(lineString, "|", Args);

    for(int k =0; Args[k] != NULL; k++){
        no_of_arguments++;
    }


    //Travers argument array
    for (int i = 0; i <no_of_arguments ; i++) {

        if (no_of_arguments > 0 && i==0){//output to pipeA if output detected
            pipe(pipeA);
        }


        //Otherwise close pipe A and use pipe B for output redirection
        if (i>0){
            intIO[0] = 1;

            if (currPipe){
                pipe(pipeB);
                close(pipeA[1]);
            }else{
                pipe(pipeA);
                close(pipeB[1]);
            }

        }

        //redirect output to pipe if last command has not been reached, ans execute the current command
        if (i < no_of_arguments-1){
            intIO[1] = 1;

        }

        executeCommand(Args[i]);
        IOreset();

        //Unused pipes closed
        if (!currPipe){
            close(pipeB[0]);
        }else{
            close(pipeA[0]);
        }

    }

    //reset commands and close all pipes
    close(pipeA[0]);
    close(pipeA[1]);
    close(pipeB[0]);
    close(pipeB[1]);
    resetCommands();


}



//parse input string to be executed
void parseLine(char* lineString){

    //Parse for output and input redirection
    lineString = outputRedirectionParser(lineString);
    lineString = inputReceiveParser(lineString);

    //parse for piping
    pipingParser(lineString);

    //Reset and move to next line
    resetCommands();


}


















