#include "shell.h"


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
    char ** parsed_args = calloc(TOKEN_LIMIT, sizeof(char **));
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

//parses the arguments for "|""
//changes the file descriptors accordingly
//executes the programs accordingly
//returns -1 if no "|" is found
int parse_pipe(char ** parsed_args) {

	//search through parsed args for "|"
	int i = 0;
	while(parsed_args[i]) {

		//if the "|" is found
		if (!strcmp(parsed_args[i], "|")) {
			
			//create a pipe
			int fds[2];
			if(pipe(fds) == -1) {
				printf("HAR.SH: %s\n", strerror(errno));
				exit(0);
			}
			int r = fds[0];
			int w = fds[1];

			//shorten it to just the first program
			char ** prog1 = malloc(sizeof(char**)*TOKEN_LIMIT);
			int x = 0;
			while(strcmp(parsed_args[x],"|")) {
				prog1[x] = parsed_args[x];
				x++;
			}

			if(!fork()) { //CHILD
				wait_for_me = getpid();

				//swap stdout with the write end of the pipe
				dup2(w,STDOUT_FILENO);

				//exec the first program into the write end of the pipe
				if(execvp(prog1[0], prog1) == -1) {
					printf("HAR.SH: %s\n", strerror(errno));
					exit(0);
				}
			} else { //PARENT

				//wait for the child to finish
				free(prog1);
				int status;
				waitpid(wait_for_me, &status, 0);
			}

			//to hold second program
			char ** prog2 = malloc(sizeof(char **)*TOKEN_LIMIT);

			//skip the pipe "|"
			x++;

			//isolate the second program in prog2
			int i = 0;
			while (parsed_args[x]) {
				prog2[i] = parsed_args[x];
				x++;
				i++;
			}


			if(!fork()) { //CHILD 2
				wait_for_me = getpid();

				close(w);

				//swap stdin with the read end of the pipe
				dup2(r,STDIN_FILENO);

				//exec the second program
				if(execvp(prog2[0], prog2) == -1) {
					printf("HAR.SH: %s\n", strerror(errno));
					exit(0);
				}
			} else { //PARENT

				//wait for the child to finish
				close(r);
				close(w);
				free(prog2);
				int status;
				waitpid(wait_for_me, &status, 0);
				return 0;
			}
		}
		i++;
	}
	//if we didn't find a pipe, continue as normal
	return -1;
}


//Takes the input as lines of commands (that have 
//already been parsed by ';') and runs said commands
//returns 0 - always
int shell (char * input) {

	//remove that pesky newline, should it exist
	newline_remover(input);
	
	//sets p to an array of tokens from the input
	char ** p = malloc(sizeof(char **) * TOKEN_LIMIT);
	p = parse_args(input);

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

    //for pipeage
	if (parse_pipe(p) > -1) {
		return 0;
	}

 	if (!fork()) {

 		//for redirection
		p = parse_redirect(p);

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
//returns 0 when run correctly
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
		if (isatty(0)) {
			printf("HAR.SH:%s:%s>>", user, path);
		}

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
	free(user);

	return 0;		
}