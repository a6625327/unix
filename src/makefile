.SUFFIXES:.c .o
CC=gcc
SRCS1=server.c rw.c lockMgr.c userLog.c
# SRCS2=client.c
SRCS2=send.c userLog.c
SRCS3=recv.c userLog.c

OBJS1=$(SRCS1:.c=.o)
OBJS2=$(SRCS2:.c=.o)
OBJS3=$(SRCS3:.c=.o)
EXEC1=serv
EXEC2=send
EXEC3=recv

LIB=-lpthread -lzlog
OPTION=-g

start:$(OBJS1) $(OBJS2) $(OBJS3)
	$(CC) -o $(EXEC1) $(OBJS1) $(LIB) $(OPTION)
	$(CC) -o $(EXEC2) $(OBJS2) $(LIB) $(OPTION)
	$(CC) -o $(EXEC3) $(OBJS3) $(LIB) $(OPTION)
	@echo "^_^ ------ OK ------ ^_^"
	rm -f *.o
.c.o:
	$(CC) -Wall -g -o $@ -c $<
clean:
	rm -f $(OBJS1)
	rm -f $(OBJS2)
	rm -f $(EXEC1)
	rm -f $(EXEC2)
	rm -f core
	rm -f recv
	rm -f tmpFile*
