//Create an HTTP server and handle requests
#include "HTTPServer.h"
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>

#define BUFFER_LENGTH 30000
#define THREAD_COUNT 2
#define NOT_FOUND "/html/notfound.html"
#define ADD_FILE_TO_PATH() strcat(path, url);

pthread_t tid[THREAD_COUNT];
pthread_mutex_t lock;
unsigned long long int request_number = 1;

static void* monitor_input(void *passed_server);
static void retrieve_page(struct HTTPRequest request, int socket);
static char * get_file_path(char *path, char *url, char *cwd, enum HTTPResponseType *response_type);
static char * execute_php(char *path, char *cwd, enum HTTPResponseType *response_type, char *parameters);
static char * read_file_into_buffer(char *path, char *cwd, enum HTTPResponseType *response_type);
static void write_response(char *buffer, int socket, enum HTTPResponseType response_type);


void launch(struct Server *server)
{
    if (pthread_mutex_init(&lock, NULL) != 0)
    {
        printf("\nFailed to initialize mutex\n");
        return;
    }

    int i = 0;
    int err;
    for (i; i < THREAD_COUNT; i++)
    {
        err = pthread_create(&(tid[i]), NULL, monitor_input, (void *)server);
        if (err != 0)
        {
            printf("\ncan't create thread :[%s]", strerror(err));
        }
    }

    i = 0;
    for (i; i < THREAD_COUNT; i++)
    {
        pthread_join(tid[i], NULL);
    }

    pthread_mutex_destroy(&lock);
}

static void* monitor_input(void *passed_server)
{
    struct Server *server = (struct Server *)passed_server;
    int addrlen = sizeof(server->address);
    long valread;
    while (1)
    {
        printf("=== AWAITING %llu ===\n", request_number);

        pthread_mutex_lock(&lock);
        int new_socket = accept(server->socket, (struct sockaddr *)&server->address, (socklen_t *)&addrlen);
        char buffer[BUFFER_LENGTH];
        valread = read(new_socket, buffer, BUFFER_LENGTH);
        request_number++;
        pthread_mutex_unlock(&lock);

        struct HTTPRequest request = http_request_constructor(buffer);
        
        retrieve_page(request, new_socket);
        
        close(new_socket);
    }
}

static void retrieve_page(struct HTTPRequest request, int socket)
{
    //Parse request
    char *url = strtok(request.request_line.search(&request.request_line, "uri", sizeof("uri")), "?");
    char *parameters = strtok(NULL, "\0");
    printf("%s\n", url);

    char *vars = strtok(NULL, "\0");

    char cwd[200];
    getcwd(cwd, sizeof(cwd));
    strcat(cwd, "/public");

    enum HTTPResponseType response_type = HTML;

    char path[strlen(cwd) + strlen(url) + strlen(NOT_FOUND)];
    get_file_path(path, url, cwd, &response_type);

    //Read requested file and write response
    char *buffer;
    if (response_type == PHP)
    {
        buffer = execute_php(path, cwd, &response_type, parameters);
    }
    else 
    {
        buffer = read_file_into_buffer(path, cwd, &response_type);
    }

    write_response(buffer, socket, response_type);

    free(buffer);
}

static char * get_file_path(char *path, char *url, char *cwd, enum HTTPResponseType *response_type)
{
    char url_copy[strlen(url)];
    strcpy(url_copy, url);

    strcpy(path, cwd);

    char *folder = strtok(url_copy, "/");
    if (folder == NULL)
    {
        strcat(path, NOT_FOUND);
    }
    else if (strcmp(folder, "html") == 0)
    {
        ADD_FILE_TO_PATH()
    }
    else if (strcmp(folder, "styles") == 0)
    {
        ADD_FILE_TO_PATH()
        *response_type = CSS;
    }
    else if (strcmp(folder, "js") == 0)
    {
        ADD_FILE_TO_PATH()
        *response_type = JS;
    }
    else if (strcmp(folder, "images") == 0)
    {
        ADD_FILE_TO_PATH()
        *response_type = PNG;
    }
    else if (strcmp(folder, "php") == 0)
    {
        ADD_FILE_TO_PATH()
        *response_type = PHP;
    }
    else
    {
        strcat(path, NOT_FOUND);
    }

    return path;
}

static char * execute_php(char *path, char *cwd, enum HTTPResponseType *response_type, char *parameters)
{
    bool file_exists = (access(path, F_OK) == 0);
    if (!file_exists)
    {
        return read_file_into_buffer(path, cwd, response_type);
    }

    *response_type == HTML;

    char cmd[1024] = {0};
    strcat(cmd, "sh Server/Nodes/RunPHP.sh ");
    strcat(cmd, path);
    if (parameters != NULL)
    {
        strcat(cmd, "\"");
        strcat(cmd, parameters);
        strcat(cmd, "\"");
    }

    char *buffer = malloc(0);
    FILE *fp = popen(cmd, "r");
    if (fp == NULL)
    {
        return read_file_into_buffer(path, cwd, response_type);
    }

    int length = 1024;
    char str[length];
    while (fgets(str, length, fp) != NULL)
    {
        buffer = realloc(buffer, sizeof(buffer) + sizeof(str));
        if (buffer == NULL)
        {
            free(buffer);
            return read_file_into_buffer(path, cwd, response_type);
        }
        strcat(buffer, str);
    }

    fclose(fp);

    return buffer;
}

static char * read_file_into_buffer(char *path, char *cwd, enum HTTPResponseType *response_type)
{
    FILE *file;
    bool file_exists = (access(path, F_OK) == 0);

    if (file_exists)
    {
        if (*response_type == PNG)
        {
            file = fopen(path, "rb");
        }
        else 
        {
            file = fopen(path, "r");
        }
    }
    if (!file_exists || file == NULL)
    {
        strcat(cwd, NOT_FOUND);
        *response_type = HTML;
        file = fopen(cwd, "r");
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *buffer = malloc(file_size + sizeof(char));
    int size = sizeof(buffer)/sizeof(char);
    for (int i = 0; i < size; i++) {
        buffer[i] = '0';
    }
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
    else if (response_type == CSS)
    {
        strcpy(response, RESPONSE_HEADERS_CSS);
    }
    else if (response_type == PHP)
    {
        strcpy(response, RESPONSE_HEADERS_HTML);
    }
    else 
    {
        strcpy(response, RESPONSE_HEADERS_PNG);
    }
    strcat(response, buffer);

    write(socket, response, sizeof(response));
}
