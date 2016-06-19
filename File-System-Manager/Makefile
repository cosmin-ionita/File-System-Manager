build: ServerConnection.o UserManager.o FileSystem.o File.o ClientConnection.o
	g++ server.cpp ServerConnection.o UserManager.o FileSystem.o File.o -lnsl -o server.exe
	g++ client.cpp ClientConnection.o -o client.exe

ServerConnection.o:
	g++ -c ServerConnection.cpp -o ServerConnection.o

ClientConnection.o:
	g++ -c ClientConnection.cpp -o ClientConnection.o

FileSystem.o:
	g++ -c FileSystem.cpp -o FileSystem.o

UserManager.o:
	g++ -c UserManager.cpp -o UserManager.o

File.o:
	g++ -c File.cpp -o File.o

run:
	./server.exe
clean:
	rm *.o server.exe client.exe
