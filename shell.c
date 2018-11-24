#include "shell.h"

int wait_for_me;

//prints garfield (sorry)
void garf() {
	int fd = open("garf", O_RDONLY);
	char * garf = malloc(sizeof(char) * GARF_SIZE);
	read(fd, garf, GARF_SIZE);
	close(fd);
	printf("%s\n", garf);
}

//takes in a string and returns a string array containing the
//tokens separated by " " from the original string
char ** parse_args(char * line) {
    char ** parsed_args = calloc(5, sizeof(char **));
    char * p = line;
    int index = 0;
    while(p != NULL) {
        parsed_args[index] = strsep(&p, " ");
        index++; 
    }
        
    return parsed_args;
}


//Takes in a string and removes the newline character (\n) from
//the end of the string if it is there
void newline_remover(char * line) {
   int newline = strlen(line) -1;
   if (line[newline] == '\n')
       line[newline] = '\0';
}


//Takes the input as lines of commands (that have 
//already been parsed by ';') and runs said commands
//returns 0 - always
int shell (char * input) {

	//remove that pesky newline, should it exist
	newline_remover(input);

	//when "exit" is typed, end this process
 	if(strcmp(input, "exit") == 0) {exit(0);}

 	//for fun
 	if(strcmp(input, "garf") == 0) {
 		garf();
 		return 0;
 	}

 	//when "cd" is typed, change to input directory
 

 	if (!fork()) {
 		wait_for_me = getpid();
    	char ** p = parse_args(input);
		if (execvp(p[0], p) == -1) {
			printf("Error: %s\n", strerror(errno));
			exit(1);
		}
	} else {
		int status;
		waitpid(wait_for_me, &status, 0);
	}
	return 0;
}


int main() {

	printf("Welcome to Garf:\n");

	garf();


	while(1) {

		printf(">>");

		//create a buffer for the incoming set of commands
		char * buff = malloc(sizeof(char) * BUFF_SIZE);
		fgets(buff, BUFF_SIZE, stdin);

		//buff with be altered, to keep track of memory
		char * free_this = buff;

		//will hold each line of commands
		char * line;

		//go through and run each line of commands as it appears
		//in order, separated by ;
		while (line = strsep(&buff, ";")) {
			shell(line);
		}

		free(free_this);

	}

	return 0;		
}