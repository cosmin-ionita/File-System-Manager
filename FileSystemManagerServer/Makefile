build: Connection.o UserManager.o
	g++ server.cpp Connection.o UserManager.o -lnsl -o server.exe

Connection.o:
	g++ -c Connection.cpp -o Connection.o

UserManager.o:
	g++ -c UserManager.cpp -o UserManager.o

run:
	./server.exe
clean:
	rm *.o server.exe
