#include "shell.h"

int wait_for_me;
char * path;

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

//takes an input for the desired path
//changes the directory to said path
//returns no value
void cd(char * path) {
	if (chdir(path) == -1) {
		printf("Error: %s\n", strerror(errno));
	}
}


//Takes the input as lines of commands (that have 
//already been parsed by ';') and runs said commands
//returns 0 - always
int shell (char * input) {

	//remove that pesky newline, should it exist
	newline_remover(input);

	//sets p to an array of tokens from the input
	char ** p = parse_args(input);

	//when "exit" is typed, end this process
 	if(strcmp(p[0], "exit") == 0) {exit(0);}

 	//for fun
 	if(strcmp(p[0], "garf") == 0) {
 		garf();
 		return 0;
 	}

 	//changes the directory to the given pathname
 	if (strcmp(p[0], "cd") == 0) {
    		cd(p[1]);
    		return 0;
    }

 	if (!fork()) {
 		wait_for_me = getpid();

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

//Begins the shell and awaits instruction from user
//to put into the shell command
//returns 0 - always
int main() {

	//intro to the shell
	printf("Welcome to Garf:\n");
	garf();

	//buffer fro the current directory name
	path = malloc(sizeof(char) * PATH_SIZE);

	while(1) {

		//keeps track of the current directory
		getcwd(path, sizeof(char) * PATH_SIZE);
		printf("GARF::%s>>", path);

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

	free(path);

	return 0;		
}