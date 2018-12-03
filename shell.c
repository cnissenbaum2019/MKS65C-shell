#include "shell.h"


int wait_for_me;
char * path;
char * user;

//prints garfield (sorry)
void garf() {
	int fd = open("garf", O_RDONLY);
	char * garf = malloc(sizeof(char) * GARF_SIZE);
	read(fd, garf, GARF_SIZE);
	close(fd);
	printf("%s\n", garf);
}

//takes out the starting spaces out of a string
//returns the string starting from the first non-space char
char * strip_spaces(char * string) {

	char * c = string;

	//ignore starting spaces
	while(*c == ' ') {
		c++;
	}
	
	return c;
}


//takes in a string and returns a string array containing the
//tokens separated by " " from the original string
char ** parse_args(char * line) {
	line = strip_spaces(line);
    char ** parsed_args = calloc(5, sizeof(char **));
    char * p = line;
    int index = 0;
    while(p != NULL) {
 		
 		parsed_args[index] = strsep(&p, " ");

    	//printf("%i: %s\n", index, parsed_args[index]);

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


//parses the arguments for <,>,>>,2>,2>>
//returns all arguments before the redirections
//changes the file descriptors accordingly
char ** parse_redirect(char ** parsed_args) {
	int index = 0;
	int redirect = -1;

	while(parsed_args[index]) {
		if (strcmp(parsed_args[index], ">") == 0) {
			int fd = open(parsed_args[index + 1], O_WRONLY | O_CREAT | O_TRUNC, 0777);
			dup2(fd,1);
			redirect = index;
		}
		if (strcmp(parsed_args[index], ">>") == 0) {
			int fd = open(parsed_args[index + 1], O_WRONLY | O_CREAT | O_APPEND, 0777);
			dup2(fd,1);
			redirect = index;
		}
		if (strcmp(parsed_args[index], "<") == 0) {
			int fd = open(parsed_args[index + 1], O_RDONLY);
			dup2(fd,0);
			redirect = index;
		}
		if (strcmp(parsed_args[index], "2>") == 0) {
			int fd = open(parsed_args[index + 1], O_WRONLY | O_CREAT | O_TRUNC, 0777);
			dup2(fd,2);
			redirect = index;
		}
		if (strcmp(parsed_args[index], "2>>") == 0) {
			int fd = open(parsed_args[index + 1], O_WRONLY | O_CREAT | O_APPEND, 0777);
			dup2(fd,2);
			redirect = index;
		}
		index++;
	}

	if (redirect > -1) {

		while(parsed_args[redirect]) {
			parsed_args[redirect] = NULL;
			redirect++;
		}

	}

	return parsed_args;
}

//parses the arguments for |
//returns all arguments after the pipe
//changes the file descriptors accordingly
char ** parse_pipe(char ** parsed_args) {
	int index = 0;
	int pipe_place = -1;
	while(parsed_args[index]) {

		if (strcmp(parsed_args[index], "|") == 0) {
			pipe_place = index;
			printf("%i\n", pipe_place);
			unsigned int fds[1];
			pipe(fds);
			

			if (!fork()) {
				printf("1st prog...\n");

				int i = 5;
				while(i >= pipe_place) {
					printf("removing %s\n", parsed_args[i]);
					parsed_args[i] == NULL;
					i--;				
				}

				dup2(fds[1],1);
				wait_for_me = getpid();

				if (execvp(parsed_args[0], parsed_args) == -1) {
					printf("HAR.SH: %s\n", strerror(errno));
					exit(0);
				}
			} else {
				int status;
				waitpid(wait_for_me, &status, 0);
			}
			dup2(fds[0],0);
		}

		index++;
	}

	char ** p = parsed_args;
	if (pipe_place > -1) {
		while(pipe_place--) {
			printf("moving forward\n");
			p++;
		}
	}
	return p++;
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

 		//for redirection
		p = parse_redirect(p);

		//for pipeage
		//p = parse_pipe(p);

 		wait_for_me = getpid();

		if (execvp(p[0], p) == -1) {
			printf("HAR.SH: %s\n", strerror(errno));
			exit(0);
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
	printf("Welcome to HAR.SH:\n");
	//garf();

	//buffer for the current directory name
	path = malloc(sizeof(char) * PATH_SIZE);

	user = malloc(sizeof(char) * USER_SIZE);

	while(1) {

		//keeps track of the current directory and user name
		getcwd(path, sizeof(char) * PATH_SIZE);
		getlogin_r(user, sizeof(char) * USER_SIZE);
		printf("HAR.SH:%s:%s>>", user, path);

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