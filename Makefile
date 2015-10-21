CC = gcc
CFLAGS = --std=c99
INCLUDES = -I/usr/include/mysql
LIBS = -L/usr/lib/mysql -lmysqlclient -lz -lpthread -ljson -lm
server: main.o process.o sql.o jparse.o threadpool.o epoll.o
	$(CC) -o server  main.o process.o sql.o jparse.o threadpool.o epoll.o $(LIBS)
main.o: main.c process.h jparse.h sql.h threadpool.h
	$(CC) -c $(CFLAGS) $(INCLUDES) main.c
jparse.o: jparse.c jparse.h
	$(CC) -c $(CFLAGS) $(INCLUDES) jparse.c
sql.o:	sql.c sql.h
	$(CC) -c $(CFLAGS) $(INCLUDES) sql.c
process.o: process.c process.h sql.h jparse.h
	$(CC) -c $(CFLAGS) $(INCLUDES) process.c
threadpool.o: threadpool.c threadpool.h
	$(CC) -c $(CFLAGS) $(INCLUDES) threadpool.c
epoll.o: epoll.c epoll.h
	$(CC) -c $(CFLAGS) $(INCLUDES) epoll.c
clean:
	rm -f server main.o process.o sql.o jparse.o threadpool.o epoll.o