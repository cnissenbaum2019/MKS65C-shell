# MKS65C-shell
For Mr.K's SLP Class

Features:
-
- exit
- commands
- semicolon(;) separation
- cd
- garf
- simple redirection (<,>)
- pipe (only one pipe)

Unsuccessful Implementations:
-
- "complex" redirection (<<,2<...)
- multiple piping

Bugs:
-
- spacing issues
   - extra spaces inbetween tokens will fail
   - extra spaces at the end of the command will fail
- wc sometimes only displays the line count (when piping)

Function Headers:
-
```
//prints garfield (sorry)
void garf();

//takes out the starting spaces out of a string
//returns the string starting from the first non-space char
char * strip_spaces(char * string);

//takes in a string and returns a string array containing the
//tokens separated by " " from the original string
char ** parse_args(char * line);

//Takes in a string and removes the newline character (\n) from
//the end of the string if it is there
void newline_remover(char * line);

//takes an input for the desired path
//changes the directory to said path
//returns no value
void cd(char * path);

//parses the arguments for <,>,>>,2>,2>>
//returns all arguments before the redirections
//changes the file descriptors accordingly
char ** parse_redirect(char ** parsed_args);

//parses the arguments for "|"
//changes the file descriptors accordingly
//executes the programs accordingly
//returns -1 if no "|" is found
int parse_pipe(char ** parsed_args);

//Takes the input as lines of commands (that have 
//already been parsed by ';') and runs said commands
//returns 0 - always
int shell (char * input);

//Begins the shell and awaits instruction from user
//to put into the shell command
//returns 0 when run correctly
int main();
```
