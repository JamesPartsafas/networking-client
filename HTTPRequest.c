#include "HTTPRequest.h"
#include <stdlib.h>
#include <string.h>

static const char body_marker = '|';

static void mark_body(char *request_string);
static int method_select(char *method);

struct HTTPRequest http_request_constructor(char *request_string)
{
    struct HTTPRequest request;

    mark_body(request_string);

    char *request_line = strtok(request_string, "\n");
    char *header_fields = strtok(NULL, body_marker);
    char *body = strtok(NULL, body_marker);

    char *method = strtok(request_line, " ");
    char *URI = strtok(NULL, " ");
    char *HTTPVersion = strtok(NULL, " ");
    HTTPVersion = strtok(HTTPVersion, "/");
    HTTPVersion = strtok(NULL, "/");

    request.Method = method_select(method);
    request.URI = URI;
    request.HTTPVersion = (float)atof(HTTPVersion);
}

static void mark_body(char *request_string)
{
    for (int i = 0; i < strlen(request_string) -1; i++)
    {
        if (request_string[i] == '\n' && request_string[i+1] == '\n')
        {
            request_string[i+1] = body_marker;
        }
    }
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