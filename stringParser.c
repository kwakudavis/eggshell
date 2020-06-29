#include "stringParser.h"

//
// Created by davis on 6/24/20.
//

//Splits string into an array of characters using a character delimiter
void parseStringByChar(char* string, char* delimiter, char** args){


    //split string content into tokens
    char* tok;
    tok = strtok(string, delimiter);

    int i=0;

    while (tok != NULL && i< MAX_ARGS){
        args[i] = tok;
        tok = strtok(NULL, delimiter);
        i++;
    }

    args[i]=NULL;



}


//Splits string into an array of characters using a string delimiter
void parseStringByString(char* string, char* delimiter, char** args){


    //Load string buffer with string content
    char stringBuffer[variableSize];
    strncpy(stringBuffer, string, variableSize);

    //Buffer to store tokenized arguments
    char retArgsBuffer[MAX_ARGS][variableSize];
    retArgsBuffer[0][0] = '\0';

    int newString = 0; //New formed string
    int newStringIndex = 0; //index in string

    //curr is the latest parsed character
    for(int currChar = 0; currChar < strlen(stringBuffer); currChar++, newStringIndex++){

        //Update returned characters array with new character
        retArgsBuffer[newString][newStringIndex] = stringBuffer[currChar];


        for(int j = 0; j+currChar < strlen(stringBuffer) && stringBuffer[j+currChar] == delimiter[j]; j++){

            if(j == strlen(delimiter)-1){
                if(retArgsBuffer[newString][0] != '\0'){
                    retArgsBuffer[newString][newStringIndex] = '\0';
                    newString++;
                    retArgsBuffer[newString][0] = '\0';
                    newStringIndex--;
                }
                currChar += j;
                continue;
            }
        }
    }

    retArgsBuffer[newString][newStringIndex] = '\0';

    retArgsBuffer[newString++][0] = '\0';

    for(int k = 0; retArgsBuffer[k][0] != '\0'; k++){
        stripSpaces(retArgsBuffer[k]);
        args[k] = retArgsBuffer[k];
    }

}