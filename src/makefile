.SUFFIXES:.c .o
CC=gcc
SRCS1=server.c
SRCS2=client.c
OBJS1=$(SRCS1:.c=.o)
OBJS2=$(SRCS2:.c=.o)
EXEC1=serv
EXEC2=cli

start:$(OBJS1) $(OBJS2)
	$(CC) -o $(EXEC1) $(OBJS1) -lpthread -g
	$(CC) -o $(EXEC2) $(OBJS2) -lpthread -g
	@echo "^_^ ------ OK ------ ^_^"
.c.o:
	$(CC) -Wall -g -o $@ -c $<
clean:
	rm -f $(OBJS1)
	rm -f $(OBJS2)
	rm -f $(EXEC1)
	rm -f $(EXEC2)