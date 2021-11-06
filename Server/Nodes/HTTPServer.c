//Create an HTTP server and handle requests
#include "HTTPServer.h"
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define BUFFER_LENGTH 30000
#define RESPONSE_HEADERS_HTML "HTTP/1.1 200 OK\nContent-Type: text/html\n\n"
#define RESPONSE_HEADERS_CSS "HTTP/1.1 200 OK\nContent-Type: text/css\n\n"

static void retrieve_page(struct HTTPRequest request, int socket);

void launch(struct Server *server)
{
    int addrlen = sizeof(server->address);
    long valread;
    while (1)
    {
        printf("=== WAITING ===\n");
        int new_socket = accept(server->socket, (struct sockaddr *)&server->address, (socklen_t *)&addrlen);
        char buffer[BUFFER_LENGTH];
        valread = read(new_socket, buffer, BUFFER_LENGTH);

        struct HTTPRequest request = http_request_constructor(buffer);

        retrieve_page(request, new_socket);
        
        close(new_socket);
    }
}

static void retrieve_page(struct HTTPRequest request, int socket)
{
    char path[BUFFER_LENGTH] = {0}; //Initialize all chars to 0

    char *url = strtok(request.request_line.search(&request.request_line, "uri", sizeof("uri")), "?");
    char *vars = strtok(NULL, "\0");

    char cwd[200];
    getcwd(cwd, sizeof(cwd));
    strcat(cwd, "/Server/public");

    strcpy(path, cwd);
    int stylesheet = 0;
    if (strcmp(url, "/test.html") == 0)
    {
        strcat(path, url);
    }
    else if (strcmp(url, "/styles.css") == 0)
    {
        strcat(path, "/styles.css");
        stylesheet = 1;
    }
    else
    {
        strcat(path, "/index.html");
    }
    //strcat(path, ".html");
    
    FILE *file = fopen(path, "r");
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *buffer = malloc(file_size + 1);
    fread(buffer, 1, file_size, file);
    fclose(file);

    char response[BUFFER_LENGTH] = {0};
    if (stylesheet)
    {
        strcpy(response, RESPONSE_HEADERS_CSS);
    }
    else {
        strcpy(response, RESPONSE_HEADERS_HTML);
    }
    strcat(response, buffer);

    write(socket, response, strlen(response));

    free(buffer);
}