PROGRAM = app.out
SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)

CC = gcc
CFLAGS = -g -Wall -Wextra -pedantic -O0
LFLAGS = -g
LIBS = -lm

all: app

app: $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) $(LIBS) -o $(PROGRAM)

clean:
	rm *.o *.d *~ $(PROGRAM)

libvm: ../lib/libvm.a

-include $(OBJS:.o=.d)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<
	$(CC) -MM $(CFLAGS) $*.c > $*.d


../lib/libvm.a: $(OBJS)
	ar rcs $@ $^
