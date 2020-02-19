CC = gcc 
CFLAGS = -g -Wall
TARGET = oss primo
OBJS = oss.o prime.o
LIBOBJS = oss.o prime.o
LIB = $(CURDIR)myGlobal.so

all : $(LIB) $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LIB)
$(LIB): $(LIBOBJS)
	$(CC) -shared -Wl,-soname,$@ -o $@ $(LIBOBJS)
oss.o : oss.c 
	$(CC) -fpic -c oss.c
prime.o : prime.c 
	$(CC) -fpic -c prime.c
.c.o:
	$(CC) $(CFLAGS) -c $<

.PHONY: clean
clean:
	rm -f *.o *~ $(LIB) $(TARGET)

