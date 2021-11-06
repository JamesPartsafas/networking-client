#include "HTTPRequest.h"
#include <stdlib.h>
#include <string.h>
#include "../DataStructures/Lists/Queue.h"

#define BODY_MARKER "|"
#define BODY_MARKER_CHAR '|'

static char* mark_body(char *request_string);
static int method_select(char *method);

struct HTTPRequest http_request_constructor(char *request_string)
{
    struct HTTPRequest request;

    request_string = mark_body(request_string);
    char request_modified[sizeof(request_string)];
    strcpy(request_modified, request_string);
    free(request_string);

    char *request_line = strtok(request_modified, "\n");
    char *header_fields = strtok(NULL, BODY_MARKER);
    char *body = strtok(NULL, BODY_MARKER);

    char *method = strtok(request_modified, " ");
    char *URI = strtok(NULL, " ");
    char *HTTPVersion = strtok(NULL, " ");
    HTTPVersion = strtok(HTTPVersion, "/");
    HTTPVersion = strtok(NULL, "/");

    request.Method = method_select(method);
    request.URI = URI;
    request.HTTPVersion = (float)atof(HTTPVersion);

    request.header_fields = dictionary_constructor(compare_string_keys);

    struct Queue headers = queue_constructor();

    char *token = strtok(header_fields, "\n");
    while (token)
    {
        headers.push(&headers, token, sizeof(*token));
        token = strtok(NULL, "\n");
    }

    char *header = (char *)headers.peek(&headers);
    while (header)
    {
        char *key = strtok(header, ":");
        char *value = strtok(NULL, "|");
        request.header_fields.insert(&request.header_fields, key, sizeof(*key), value, sizeof(*value));
        headers.pop(&headers);
        header = (char *)headers.peek(&headers);
    }
}

static char* mark_body(char *request_string)
{
    char *request_copy = malloc(sizeof(request_string));
    strcpy(request_copy, request_string);

    for (int i = 0; i < strlen(request_copy) -1; i++)
    {
        if (request_copy[i] == '\n' && request_copy[i+1] == '\n')
        {
            request_copy[i+1] = BODY_MARKER_CHAR;
        }
    }

    return request_copy;
}

static int method_select(char *method)
{
    if (strcmp(method, "GET") == 0)
    {
        return GET;
    }
    else if (strcmp(method, "POST") == 0)
    {
        return POST;
    }
    else if (strcmp(method, "PUT") == 0)
    {
        return PUT;
    }
    else if (strcmp(method, "HEAD") == 0)
    {
        return HEAD;
    }
    else if (strcmp(method, "PATCH") == 0)
    {
        return PATCH;
    }
    else if (strcmp(method, "DELETE") == 0)
    {
        return DELETE;
    }
    else if (strcmp(method, "CONNECT") == 0)
    {
        return CONNECT;
    }
    else if (strcmp(method, "OPTIONS") == 0)
    {
        return OPTIONS;
    }
    else if (strcmp(method, "TRACE") == 0)
    {
        return TRACE;
    }
}