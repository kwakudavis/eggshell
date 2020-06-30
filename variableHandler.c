#include "variableHandler.h"


//
// Created by davis on 6/24/20.
//


//Sets the value of a variable
void setVariable(char* variableName, char* value){

    //iterate variable container to update variable if variable already exists

    for(int j=0; j<variableCount; j++){
        if(strncmp(variablesContainer[j]->key, variableName, variableSize) == 0){
            strncpy(variablesContainer[j]->value, value, variableSize);
                return;
        }
    }//create a new variable if variable does not exist already


    //increment size of container to accommodate new variable
    variableCount++;
    variablesContainer = realloc(variablesContainer, variableCount * sizeof(struct variables**));
    variablesContainer[variableCount-1]= malloc(sizeof(struct variables));

    //set Key and value of variable
    strncpy(variablesContainer[variableCount-1]->key, variableName, variableSize);
    strncpy(variablesContainer[variableCount-1]->value, value, variableSize);
}


//Returns the value of a variable given its name
char* getVariable(char* variableName){

    //Traverse through variable container to find variable if it exists in the variables array
    for(int j = 0; j < variableCount; j++){
        if(strncmp(variablesContainer[j]->key, variableName, variableSize) == 0){
            return variablesContainer[j]->value;

        }
    } //else return NULL

    return NULL;

}



//Strips of first character from string
char* stripFirstCharacter(char* string){

    char buffer[variableSize];
    for(int k=0; string[k] != '\0' && string[k] != '\n'; k++){
        //update buffer with string characters
        buffer[k] = string[k+1];
    }
    char* stringBuffer = buffer;
    return stringBuffer;

}


//Sets up the Terminal, CWD and SHELL variables on initialisation
void initializeVariables(char* shellName){

    //Set CWD variable if its not NULL
    char CWDBuffer[variableSize];
   if(getcwd(CWDBuffer, variableSize) == NULL){
        perror("Unable to fetch current working directory");
        exit(-1);

    }  else{//Print error if its NULL


        setVariable("CWD",CWDBuffer);

           //extract shell name from CWD variable
           char shellBuffer[variableSize];
        strncpy(shellBuffer, getVariable("CWD"), variableSize);
        // strncat(shellBuffer,stripFirstCharacter(shellName), variableSize);
        // setVariable("SHELL", shellName);


       }




    //Set up Terminal variable if its not null, else return an error.
    if(ttyname(STDIN_FILENO) == NULL){

        perror("Unable to fetch ttyname with ERROR: ");
        exit(atoi(getVariable("EXITCODE")));


    }else{
        setVariable("TERMINAL", ttyname(STDIN_FILENO));
    }



}



//Remove spaces from front and back
void  stripSpaces(char* string){
    char stringBuffer[variableSize];

    int i = 0;
    int count = 0;


    //Skip until character value is not NULL
    while(stringBuffer[i] == ' '){
        i++;
    }

    //Copy string into string buffer
    while( i<variableSize && i< strlen(string)){
        stringBuffer[count] = string[i];
        i++;
        count++;

    }

    stringBuffer[count] = '\0';

    //Replace end spaces with "\0"
    for(int j = count-1; j >= 0; j-- ){
        if (stringBuffer[j] == ' '){
            stringBuffer[j] = '\0';
        }else{
            break;
        }
    }

    //Update string
    strncpy(string, stringBuffer, variableSize);

}