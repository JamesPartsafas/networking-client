//Create an HTTP server and handle requests
#include "HTTPServer.h"
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define BUFFER_LENGTH 30000
#define NOT_FOUND "/notfound.html"

static void retrieve_page(struct HTTPRequest request, int socket);
static char * get_file_path(char *url, char *path, enum HTTPResponseType *response_type); //TODO
static char * read_file_into_buffer(char *path, char *cwd, enum HTTPResponseType *response_type);
static void write_response(char *buffer, int socket, enum HTTPResponseType response_type);


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
    char path[300] = {0}; //Initialize all chars to 0

    char *url = strtok(request.request_line.search(&request.request_line, "uri", sizeof("uri")), "?");
    char *vars = strtok(NULL, "\0");

    char url_copy[strlen(url)];
    strcpy(url_copy, url);

    char cwd[200];
    getcwd(cwd, sizeof(cwd));
    strcat(cwd, "/public");

    strcpy(path, cwd);
    enum HTTPResponseType response_type = HTML;

    //Find requested file type
    char *folder = strtok(url_copy, "/");
    char *filename = strtok(NULL, "/");
    if (folder == NULL)
    {
        strcat(path, NOT_FOUND);
    }
    else if (strcmp(folder, "html") == 0)
    {
        strcat(path, url);
    }
    else if (strcmp(folder, "styles") == 0)
    {
        strcat(path, url);
        response_type = CSS;
    }
    else
    {
        strcat(path, NOT_FOUND);
    }

    //Read file and write response
    char *buffer = read_file_into_buffer_testing(path, cwd, &response_type);
    write_response(buffer, socket, response_type);
}

static char * read_file_into_buffer(char *path, char *cwd, enum HTTPResponseType *response_type)
{
    FILE *file;

    //Check if file exists
    if (access(path, F_OK) == 0)
    {
        file = fopen(path, "r");
    }
    else 
    {
        strcat(cwd, NOT_FOUND);
        *response_type = HTML;
        file = fopen(cwd, "r");
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *buffer = malloc(file_size + 1);
    fread(buffer, 1, file_size, file);
    fclose(file);

    return buffer;
}

static void write_response(char *buffer, int socket, enum HTTPResponseType response_type)
{
    char response[BUFFER_LENGTH] = {0};
    if (response_type == HTML)
    {
        strcpy(response, RESPONSE_HEADERS_HTML);
    }
    else {
        strcpy(response, RESPONSE_HEADERS_CSS);
    }
    strcat(response, buffer);

    write(socket, response, strlen(response));

    free(buffer);
}