make: shell.c shell.h garf
	gcc -o Shell shell.c

run: Shell garf
	./Shell

clean: Shell
	rm Shell

Shell:
	 gcc -o Shell shell.c