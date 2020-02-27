CC = gcc 
CFLAGS = -g -Wall
TARGET = oss
TARGET1 = prime
OBJS = oss.o
OBJS1 = prime.o

.SUFFIXES: .c .o

all : $(TARGET) $(TARGET1)

$(TARGET1): $(OBJS1)
	$(CC)  $(OBJS1) -o $(TARGET1)

$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS)

.c.o:
	$(CC) $(CFLAGS) -c $<

.PHONY: clean
clean:
	rm -f *.o *.log *~ $(LIB) $(TARGET) $(OBJS) $(OTHERLIB) $(TARGET1)

