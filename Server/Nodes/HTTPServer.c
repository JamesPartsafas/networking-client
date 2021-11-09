//Create an HTTP server and handle requests
#include "HTTPServer.h"
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define BUFFER_LENGTH 30000
#define NOT_FOUND "/notfound.html"
#define ADD_FILE_TO_PATH() strcat(path, url);

static void retrieve_page(struct HTTPRequest request, int socket);
static char * get_file_path(char *path, char *url, char *cwd, enum HTTPResponseType *response_type);
static char * read_file_into_buffer(char *path, char *cwd, enum HTTPResponseType *response_type);
static void write_response(char *buffer, int socket, enum HTTPResponseType response_type);


void launch(struct Server *server)
{
    unsigned long long int request_number = 1;
    int addrlen = sizeof(server->address);
    long valread;
    while (1)
    {
        printf("=== AWAITING %llu ===\n", request_number);
        int new_socket = accept(server->socket, (struct sockaddr *)&server->address, (socklen_t *)&addrlen);
        char buffer[BUFFER_LENGTH];
        valread = read(new_socket, buffer, BUFFER_LENGTH);

        struct HTTPRequest request = http_request_constructor(buffer);

        retrieve_page(request, new_socket);
        
        close(new_socket);

        request_number++;
    }
}

static void retrieve_page(struct HTTPRequest request, int socket)
{
    //Parse request
    char *url = strtok(request.request_line.search(&request.request_line, "uri", sizeof("uri")), "?");
    char *vars = strtok(NULL, "\0");

    char cwd[200];
    getcwd(cwd, sizeof(cwd));
    strcat(cwd, "/public");

    enum HTTPResponseType response_type = HTML;

    char path[strlen(cwd) + strlen(url) + strlen(NOT_FOUND)];
    get_file_path(path, url, cwd, &response_type);

    //Read requested file and write response
    char *buffer = read_file_into_buffer(path, cwd, &response_type);

    write_response(buffer, socket, response_type);

    free(buffer);
}

static char * get_file_path(char *path, char *url, char *cwd, enum HTTPResponseType *response_type)
{
    char url_copy[strlen(url)];
    strcpy(url_copy, url);

    strcpy(path, cwd);

    char *folder = strtok(url_copy, "/");
    char *filename = strtok(NULL, "/");
    if (folder == NULL)
    {
        strcat(path, NOT_FOUND);
        printf("in null %s", path);
    }
    else if (strcmp(folder, "html") == 0)
    {
        ADD_FILE_TO_PATH()
        printf("in html %s", path);
    }
    else if (strcmp(folder, "styles") == 0)
    {
        ADD_FILE_TO_PATH()
        *response_type = CSS;
        printf("in css %s", path);
    }
    else if (strcmp(folder, "js") == 0)
    {
        ADD_FILE_TO_PATH()
        *response_type = JS;
        printf("in js %s", path);
    }
    else if (strcmp(folder, "images") == 0)
    {
        ADD_FILE_TO_PATH()
        *response_type = PNG;
        printf("in png %s", path);
    }
    else
    {
        strcat(path, NOT_FOUND);
        printf("in not found %s", path);
    }

    return path;
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

    char *buffer = malloc(file_size + sizeof(char));
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
    else if (response_type == JS)
    {
        strcpy(response, RESPONSE_HEADERS_JS);
    }
    else if (response_type == PNG)
    {
        strcpy(response, RESPONSE_HEADERS_PNG);
    }
    else {
        strcpy(response, RESPONSE_HEADERS_CSS);
    }
    strcat(response, buffer);

    write(socket, response, strlen(response));
}
