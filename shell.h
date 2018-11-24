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

void garf();

char ** parse_args(char * line);

void newline_remover(char * line);

int shell(char * input);
