CC=gcc

all: Client DataStructures

clean: ClearClient ClearDataStructuresLibrary

# Create server related files
EXECUTABLE=Server.bin
Client:
	$(CC) Server/test.c Server/Server.c Server/HTTPRequest.c -o $(EXECUTABLE)

ClearClient:
	rm $(EXECUTABLE)

# Creates the data structures library
LIBRARY=DataStructures/DataStructures.a
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
	$(CC) -c DataStructures/Common/Node.c

LinkedList:
	$(CC) -c DataStructures/Lists/LinkedList.c

Queue:
	$(CC) -c DataStructures/Lists/Queue.c
	
BinarySearchTree:
	$(CC) -c DataStructures/Trees/BinarySearchTree.c

Entry:
	$(CC) -c DataStructures/Dictionary/Entry.c

Dictionary:
	$(CC) -c DataStructures/Dictionary/Dictionary.c
