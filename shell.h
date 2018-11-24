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

//prints garfield (sorry)
void garf();

//takes in a string and returns a string array containing the
//tokens separated by " " from the original string
char ** parse_args(char * line);

//Takes the input as lines of commands (that have 
//already been parsed by ';') and runs said commands
//returns 0 - always
void newline_remover(char * line);

//Begins the shell and awaits instruction from user
//to put into the shell command
//returns 0 - always
int shell(char * input);
