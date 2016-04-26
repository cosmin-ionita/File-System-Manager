build: Connection.o
	g++ server.cpp Connection.o -lnsl -o server.exe

Connection.o:
	g++ -c Connection.cpp -o Connection.o
run:
	./server.exe
clean:
	rm *.o server.exe
