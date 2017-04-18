all: mailbox.o server.o client.o
	gcc -o server mailbox.o server.o -lrt
	gcc -o client mailbox.o client.o -lrt

mailbox.o: mailbox.c mailbox.h
	gcc -c mailbox.c -lrt

server.o: server.c mailbox.c mailbox.h
	gcc -c server.c -lrt

client.o: client.c mailbox.c mailbox.h
	gcc -c client.c -lrt 

clean:
	rm -rf *.o
	rm server client

