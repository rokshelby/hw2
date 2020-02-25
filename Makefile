CC = gcc 
CFLAGS = -g -Wall
TARGET = oss
TARGET1 = prime
OBJS = oss.o prime.o
OBJS1 = prime.o
LIBOBJS = oss.o prime.o
LIBOBJS1 = prime.o
OTHERLIB = libglob.a
all : $(LIB) $(OTHERLIB) $(TARGET) $(TARGET1)

.SUFFIXES: .c .o
$(TARGET1): $(OBJS1)
	$(CC)  $(OBJS1) -o $(TARGET1)

.SUFFIXES: .c .o
$(TARGET): $(OBJS)
	$(CC) -o $@ -L/$(CURDIR)/ -lglob -o $(TARGET)


$(LIB): $(LIBOBJS)
	$(CC) -shared -Wl,-soname,$@ -o $@ $(LIBOBJS)


$(OTHERLIB): $(LIBOBJS)
	ar cr $(OTHERLIB) $(LIBOBJS)	
.c.o:
	$(CC) $(CFLAGS) -c $<

.PHONY: clean
clean:
	rm -f *.o *~ $(LIB) $(TARGET) $(OBJS) $(OTHERLIB) $(TARGET1)

