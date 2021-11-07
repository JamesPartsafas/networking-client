The project above contains a web server that can serve responses to HTTP requests. It can be modified to handle other types of network protocols.

The build the project, clone it onto a Unix system and run the `make all` command. A binary file `Server.bin` will be outputted that can be run with sudo privilege. To modify the responses sent to clients, alter the `public` folder file contents as necessary.

The libs folder contains external libraries used to provide basic data structures that are used by the server, such as hash maps and queues.