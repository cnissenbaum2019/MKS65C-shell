# MKS65C-shell
For Mr.K's SLP Class

Features that Work:
-
- Exit
- Commands
- Semicolon separation
- garf

Features that I plan to implement:
-
- cd

Bugs:
-
- commands following a space are not recognized

Function Headers:
-
```
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
```
