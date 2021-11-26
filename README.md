# Networking Server
A modular, multi-threaded web server written from scratch in C that can provide static content over HTTP and can run backend scripts through `PHP-CGI`. It can be easily modified to handle other networking protocols, such as FTP or SSH.

### General Info
The server can be used to host websites that serve static or dynamic content. HTML, CSS, JavaScript, and image files can all be sent to the client. Additionally, PHP scripts can be run, allowing for more complex websites. At the moment, the server only has support for HTTP messaging to clients, but it is easily extensible and modifiable, allowing for support to be added in easily for any needed networking protocol.

### Development
The build the project, clone it onto a Unix system and run the `make all` command. A binary file `Server.bin` will be outputted that can be run with sudo privilege. The server can be communicated with from any web browser or other HTTP client at `localhost:80`. To modify the responses sent to clients, alter the `public` folder file contents as necessary. To extend the server with additional protocols, such as FTP, simply alter the `launch` function pointer passed into the `Server` struct in main.c and have it point to the desired launch sequence.

### Libraries
The libs folder contains external libraries used to provide basic data structures that are used by the server, such as hash maps and queues.
