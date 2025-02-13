CC=gcc

all: DataStructures Client

clean: ClearClient ClearDataStructuresLibrary

# Create server related files
EXECUTABLE=Server.bin
LIBRARY=libs/DataStructures/DataStructures.a
Client:
	$(CC) Server/main.c Server/Server.c Server/Protocols/HTTPRequest.c Server/Nodes/HTTPServer.c $(LIBRARY) -pthread -o $(EXECUTABLE)

ClearClient:
	rm $(EXECUTABLE)

# Creates the data structures library
DataStructures: CreateDataStructures ClearDataStructures

CreateDataStructures: DataStructuresSub
	ar rcs $(LIBRARY) Node.o LinkedList.o Queue.o BinarySearchTree.o Entry.o Dictionary.o
	
ClearDataStructures:
	rm Node.o LinkedList.o Queue.o BinarySearchTree.o Entry.o Dictionary.o

ClearDataStructuresLibrary:
	rm $(LIBRARY)

# Sub components of the data structures library
DataStructuresSub: Node LinkedList Queue BinarySearchTree Entry Dictionary

Node:
	$(CC) -c libs/DataStructures/Common/Node.c

LinkedList:
	$(CC) -c libs/DataStructures/Lists/LinkedList.c

Queue:
	$(CC) -c libs/DataStructures/Lists/Queue.c
	
BinarySearchTree:
	$(CC) -c libs/DataStructures/Trees/BinarySearchTree.c

Entry:
	$(CC) -c libs/DataStructures/Dictionary/Entry.c

Dictionary:
	$(CC) -c libs/DataStructures/Dictionary/Dictionary.c
