#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#define BUFF_SIZE   100
#define TOKEN_LIMIT  10
#define GARF_SIZE   330
#define PATH_SIZE   200

//prints garfield (sorry)
void garf();

//takes in a string and returns a string array containing the
//tokens separated by " " from the original string
char ** parse_args(char * line);

//Takes the input as lines of commands (that have 
//already been parsed by ';') and runs said commands
//returns 0 - always
void newline_remover(char * line);

//takes an input for the desired path
//changes the directory to said path
//returns no value
void cd(char * path);

//Begins the shell and awaits instruction from user
//to put into the shell command
//returns 0 - always
int shell(char * input);

//Begins the shell and awaits instruction from user
//to put into the shell command
//returns 0 - always
int main();
