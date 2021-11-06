#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h> //for exit
#include "Server.h"

void launch(struct Server *server) {
    char buffer[30000];
    int address_length = sizeof(server->address);
    int new_socket;
    char *hello;

    while(1) {
        printf("======= WAITING FOR CONNECTION =======\n");

        new_socket = accept(server->socket, (struct sockaddr *)&server->address, (socklen_t *)&address_length); //Should run check that connection succeeded
        read(new_socket, buffer, 30000);

        printf("%s\n", buffer);
        hello = "HTTP/1.1 200 OK\nDate: Mon, 27 Jul 2009 12:28:53 GMT\nServer: Apache/2.2.14 (Win32)\nLast-Modified: Wed, 22 Jul 2009 19:15:56 GMT\nContent-Type: text/html\nConnection: Closed\n\n<html><body><h1>Hello, World!</h1></body></html>";
        size_t buffer_remaining = strlen(hello);

        while (buffer_remaining > 0) {
            ssize_t written = write(new_socket, hello, buffer_remaining);
            if (written < 0) {
                printf("Failure writing;");
                exit(1);
            }

            buffer_remaining -= written;
            hello  = ((char *) hello) + written;
        }
        
        close(new_socket);
    }

}

int main() {
    struct Server server = server_constructor(AF_INET, SOCK_STREAM, 0, INADDR_ANY, 80, 10, launch);
    server.launch(&server);
}