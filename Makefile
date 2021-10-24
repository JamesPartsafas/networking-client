CC=gcc

main:
	$(CC) test.c Server.c -o test

clean:
	rm test