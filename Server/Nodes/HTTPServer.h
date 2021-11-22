//Create an HTTP server and handle requests
#ifndef HTTPServer_h
#define HTTPServer_h

#include "../Server.h"
#include "../Protocols/HTTPRequest.h"

//Textual responses
#define RESPONSE_HEADERS_HTML "HTTP/1.1 200 OK\nContent-Type: text/html\n\n"
#define RESPONSE_HEADERS_CSS "HTTP/1.1 200 OK\nContent-Type: text/css\n\n"
#define RESPONSE_HEADERS_JS "HTTP/1.1 200 OK\nContent-Type: text/javascript\n\n"

//Image responses
#define RESPONSE_HEADERS_JPEG "HTTP/1.1 200 OK\nContent-Type: image/jpeg\n\n"
#define RESPONSE_HEADERS_PNG "HTTP/1.1 200 OK\nContent-Type: image/png\n\n"

//Error responses
#define RESPONSE_NOT_FOUND "HTTP/1.1 404 Not Found\nContent-Type: text/html\n\n"

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

enum HTTPResponseType
{
    HTML,
    CSS,
    JS,
    PNG,
    PHP
};

struct HTTPServer http_server_constructor(void);
void launch(struct Server *server);

#endif