//Used to parse HTTP requests and pull out relevant data
#include "HTTPRequest.h"
#include <stdlib.h>
#include <string.h>

#define BODY_MARKER "|"
#define BODY_MARKER_CHAR '|'
#define DICT_CONSTRUCTOR() dictionary_constructor(compare_string_keys)

static void extract_request_line_data(struct HTTPRequest *request, char *request_line);
static void extract_header_data(struct HTTPRequest *request, char *header_fields);
static void extract_body_data(struct HTTPRequest *request, char *body);

// CONSTRUCTORS AND DESTRUCTORS

//Create request struct
struct HTTPRequest http_request_constructor(char *request_string)
{
    struct HTTPRequest request;

    //Mark body separation
    int request_length = strlen(request_string);
    char requested[request_length];
    strcpy(requested, request_string);
    for (int i = 0; i < request_length; i++)
    {
        if (requested[i] == '\n' && requested[i+ 1] == '\n')
        {
            requested[i + 1] = BODY_MARKER_CHAR;
            break;
        }
    }

    //Separate request into sections and parse
    char *request_line = strtok(requested, "\n");
    char *header_fields = strtok(NULL, BODY_MARKER);
    char *body = strtok(NULL, BODY_MARKER);

    extract_header_data(&request, header_fields);
    extract_request_line_data(&request, request_line);
    extract_body_data(&request, body);

    return request;
}

//Free data
void http_request_destructor(struct HTTPRequest *request)
{
    dictionary_destructor(&request->request_line);
    dictionary_destructor(&request->header_fields);
    dictionary_destructor(&request->body);
}

// PRIVATE METHODS

//Retrieves method, URI, and HTTP version from request
static void extract_request_line_data(struct HTTPRequest *request, char *request_line)
{
    char request_line_fields[strlen(request_line)];
    strcpy(request_line_fields, request_line);

    char *method = strtok(request_line_fields, " ");
    char *uri = strtok(NULL, " ");
    char *http_version = strtok(NULL, "\0");

    struct Dictionary request_line_dictionary = DICT_CONSTRUCTOR();

    request_line_dictionary.insert(&request_line_dictionary, "method", sizeof("method"), method, sizeof(char[strlen(method)]));
    request_line_dictionary.insert(&request_line_dictionary, "uri", sizeof("uri"), uri, sizeof(char[strlen(uri)]));
    request_line_dictionary.insert(&request_line_dictionary, "http_version", sizeof("http_version"), http_version, sizeof(char[strlen(http_version)]));

    request->request_line = request_line_dictionary;

    //Pull out request params
    if (request->request_line.search(&request->request_line, "GET", sizeof("GET")))
    {
        extract_body_data(request, (char *)request->request_line.search(&request->request_line, "uri", sizeof("uri")));
    }
}

//Retrieves header fields
static void extract_header_data(struct HTTPRequest *request, char *header_fields)
{
    char fields[strlen(header_fields)];
    strcpy(fields, header_fields);

    struct Queue headers = queue_constructor();
    char *field = strtok(fields, "\n");
    while (field)
    {
        headers.push(&headers, field, sizeof(char[strlen(field)]));
        field = strtok(NULL, "\n");
    }

    request->header_fields = DICT_CONSTRUCTOR();

    char *header = (char *)headers.peek(&headers);
    while (header)
    {
        char *key = strtok(header, ":");
        char *value = strtok(NULL, "\0");
        if (value)
        {
            if (value[0] == ' ')
            {
                value++;
            }
            request->header_fields.insert(&request->header_fields, key, sizeof(char[strlen(key)]), value, sizeof(char[strlen(value)]));
        }
        headers.pop(&headers);
        header = (char *)headers.peek(&headers);
    }
    queue_destructor(&headers);
}

//Retrieves body data
static void extract_body_data(struct HTTPRequest *request, char *body)
{
    //Check for a Content-Type header to determine flow
    char *content_type = (char *)request->header_fields.search(&request->header_fields, "Content-Type", sizeof("Content-Type"));
    if (content_type)
    {
        struct Dictionary body_fields = DICT_CONSTRUCTOR();
        if (strcmp(content_type, "application/x-www-form-urlencoded") == 0)
        {
            //Save all key value pairs in queue, then move them over to dictionary
            struct Queue queue_fields = queue_constructor();
            char *body_field = strtok(body, "&");
            while (body_field)
            {
                queue_fields.push(&queue_fields, body_field, sizeof(char[strlen(body_field)]));
                body_field = strtok(NULL, "&");
            }

            body_field = queue_fields.peek(&queue_fields);
            while (body_field)
            {
                char *key = strtok(body_field, "=");
                char *value = strtok(NULL, "\0");
                if (value[0] == ' ')
                {
                    value++;
                }

                body_fields.insert(&body_fields, key, sizeof(char[strlen(key)]), value, sizeof(char[strlen(value)]));

                queue_fields.pop(&queue_fields);
                body_field = queue_fields.peek(&queue_fields);
            }

            queue_destructor(&queue_fields);
        }
        else
        {
            //Save data as a single pair
            body_fields.insert(&body_fields, "data", sizeof("data"), body, sizeof(char[strlen(body)]));
        }

        request->body = body_fields;
    }
}