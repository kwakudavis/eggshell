//
// Created by davis on 6/27/20.
//

#include "commandsHandler.h"
#include "processManager.h"
#include "stringParser.h"


//Execute change directory command
void cdCommand(char* DIR){

    if(chdir(DIR) == 0){ //If change in directory us a success, updated CWD variable. Else print error
        char CWDBuffer[variableSize];
        getcwd(CWDBuffer, variableSize);
        setVariable("CWD", CWDBuffer);
    }else{
        fprintf(stderr, "PATH INVALID\n");

    }

}



//Print all shell and enviroment variables
void showenvCommand(){

    int j=0;
    while (j < variableCount){

        printf("%s => %s\n",variablesContainer[j]->key, variablesContainer[j]->value);

        j++;
    }

}




//Update content of variable
void updateVarCommand(char* lineString){

    char * variableUpdateBuffer[2];

    //Parse string to get content of variable
    parseStringByChar(lineString, "=", variableUpdateBuffer);


    //IF no update value is assigned return with an error
    if (variableUpdateBuffer[1] == NULL){
        fprintf(stderr, "Invalid assignment\n");
        return;

    }

    char variableValues[2][variableSize];

    strncpy(variableValues[0], variableUpdateBuffer[0],  variableSize);
    strncpy(variableValues[1], variableUpdateBuffer[1], variableSize);


    //CLean unformated variable strings

    strncpy(variableValues[0], stripFirstCharacter(variableValues[0]), variableSize);
    stripSpaces(variableValues[0]);
    stripSpaces(variableValues[1]);


    //If variable string begins another $ trim before copying

    if (variableValues[1][0] == '$'){
        strncpy(variableValues[1], stripFirstCharacter(variableValues[1]), variableSize);

        if (getVariable(variableValues[1]) == NULL){//IF variable does not exist, return an error
          printf("Error:Variable not found");
            return;
        }else{
            strncpy(variableValues[1], getVariable(variableValues[1]), variableSize);
        }

    }


}




//Executes source command
void sourceCommand(char* filename, void recursiveCallbackFunc(char* lineString)){

        if(filename == NULL){//if file name is NULL return error
            fprintf(stderr, "Invalid file name\n");
        }else{

            FILE *fp
                //Load file and read file content
            char fileName[variableSize];

            strncpy(fileName, getVariable("CWD"), variableSize);

            strncat(fileName, "/", variableSize);

            strncat(fileName, filename, variableSize);

            if((fp = fopen(fileName, "r"))){

                char funcPtr[variableSize];
                    //Execute each line
                for(int j= 0; fgets(funcPtr, variableSize, fp); j++){
                    funcPtr[strlen(funcPtr)-1] = '\0';
                    recursiveCallbackFunc(funcPtr);
                }

            }else{
                fprintf(stderr, "Error: file not found\n");
            }

            fclose(fp);

        }


}



//Forks child
void forkChildCommand(char** Args, int* pipeInput, int* pipeOutput, bool readPipe, bool writePipe) {

    //Holds the state of the background process
    bool Wait = true;


    //IF the charecter & is detected as a parameter then set background process state to false else otherwise.
    for (int j = 0; Args[j] != NULL; j++) {

        if (strncmp(Args[j], "&", variableSize) == 0) {
            Args[j] = NULL;
            Wait = false;
            break;
        } else {
            Wait = true;
        }

    }


    pid_t cPID = fork();
    char execPaths[variableSize]; //Holds executable paths
    char execPathHelper[variableSize]; //supports the exec path Array

    if (cPID > 0) {

        addProc(cPID);
        if (Wait) {
            waitForChildProc();
        }
    } else if (cPID == 0) {


        signal(SIGINT, SIG_IGN);
        signal(SIGTSTP, SIG_IGN);

        strncpy(execPaths, getVariable("CWD"), variableSize);
        strncat(execPaths, ":", variableSize);
        strncat(execPaths, getVariable("PATH"), variableSize);





    //Iterate through all possible paths

        for (int i = 0; execPaths[i] != NULL; i++) {

            strncpy(execPathHelper, execPaths[i], variableSize);
            strncat(execPathHelper, "/", variableSize);
            strncat(execPathHelper, Args[0], variableSize);

            execPaths[i] = execPathHelper;




                    if (writePipe) {
                        dup2(pipeOutput[1], STDOUT_FILENO);
                    }

                    if (readPipe) {
                        dup2(pipeInput[0], STDOUT_FILENO);
                    }

                    execv(execPaths[i], Args);
                    }

                    perror("Failed to execute path");
                    exit(-1);

    }else{

            fprintf(stderr, "Failed to Fork ");
    }

}



//Executes echo command
void echoCommand(char** Args){

    //Determines whether argument is a text
    bool containsQoute = 0;


    for (int i = 1; Args[i] != NULL ; ++i) {

        if(containsQoute == false){

            if (Args[i][0]=='$' || Args[i][0] == '"'){
                //Trim the first letter

                char s[variableSize];
                strncpy(s, stripFirstCharacter(Args[i]), variableSize);

                if(Args[i][0] == '"'){
                    containsQoute = true;
                    for (int j = 0; Args[i][j] != '\0'; j++) {
                        if ( Args[i][j] == '"' && Args[i][j+1] == '\0'){

                            containsQoute  = false;
                            s[j-1] = '\0';
                            continue;
                        }
                        s[j-1] =Args[i][j];
                    }

                    if(Args[i+1] == NULL){
                        printf("%s ", s);
                    }else{
                        printf("%s ", s);
                    }
                }

                    else if(Args[i][0] == '$'){

                        if (Args[i+1] == NULL){

                            printf("%s", getVariable(s));
                        }else{
                            printf("%s", getVariable(s));

                        }
                    }

            }else{

                char ss[variableSize];

                for (int k = 0; Args[i][k] != '\0' ; ++k) {

                    if (Args[i][k] == '"' && Args[i][k+1] == '\0'){
                       containsQoute = false;

                        ss[k] = '\0';
                        continue;
                    }
                    ss[k] = Args[i][k];
                }

                if (Args[i+1 == NULL]){
                    printf("%s", ss);
                }else{
                    printf("%s", ss);
                }


            }

        }
                printf("\n");
    }










}