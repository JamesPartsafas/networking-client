//Create an HTTP server and handle requests
#ifndef HTTPServer_h
#define HTTPServer_h

#include "../Server.h"
#include "../Protocols/HTTPRequest.h"

struct HTTPServer
{
    struct  Server server;
    struct Dictionary routes;
};

enum HTTPMethods
{
    CONNECT,
    DELETE,
    GET,
    HEAD,
    OPTIONS,
    PATCH,
    POST,
    PUT,
    TRACE
};

struct HTTPServer http_server_constructor(void);
void launch(struct Server *server);

#endif