CC=gcc

main:
	$(CC) test.c Server.c HTTPRequest.c -o test

clean:
	rm test