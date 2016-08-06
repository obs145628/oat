PROGRAM = app.out
SRCS = $(wildcard *.cc)
OBJS = $(SRCS:.cc=.o)

CC = g++
CFLAGS = -g -Wall -Wextra -pedantic -O0 -std=c++14
LFLAGS = -g
LIBS = -lpthread -lboost_system -lboost_thread -lboost_filesystem

all: app

app: $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) $(LIBS) -o $(PROGRAM)

clean:
	rm *.o *.d *~ $(PROGRAM)

-include $(OBJS:.o=.d)

%.o: %.cc
	$(CC) $(CFLAGS) -o $@ -c $<
	$(CC) -MM $(CFLAGS) $*.cc > $*.d
