#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

//
// Created by davis on 6/24/20.
//

#define MAX_ARGS 64
#define noOfVariables 8
#define variableSize 512

struct variables { char key[variableSize], value[variableSize] };
struct variables ** variablesContainer;
int variableCount;

//Sets up the Terminal, CWD and SHELL variables on initialisation
void initializeVariables(char* shellName);

//Sets the value of a variable
void setVariable(char* variableName, char* value);

//Returns the value of a variable given its name
char* getVariable(char* variableName);

//Strips of first character from string
char* stripFirstCharacter(char* string);

//Remove spaces from front and back
void  stripSpaces(char* string);
#ifndef EGGSHELL_VARIABLEHANDLER_H
#define EGGSHELL_VARIABLEHANDLER_H







#endif //EGGSHELL_VARIABLEHANDLER_H



