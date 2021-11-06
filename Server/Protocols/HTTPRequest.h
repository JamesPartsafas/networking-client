//Used to parse HTTP requests and pull out relevant data
#ifndef HTTPRequest_h
#define HTTPRequest_h

#include "../../DataStructures/DataStructures.h"

struct HTTPRequest
{
    struct Dictionary request_line;
    struct Dictionary header_fields;
    struct Dictionary body;
};

struct HTTPRequest http_request_constructor(char *request_string);
void http_request_destructor(struct HTTPRequest *request);

#endif