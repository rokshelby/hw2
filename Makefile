CC = gcc 
CFLAGS = -g -Wall
TARGET = oss
OBJS = oss.o prime.o
LIBOBJS = oss.o prime.o
OTHERLIB = libglob.a
all : $(LIB) $(OTHERLIB) $(TARGET) 

$(TARGET): $(OBJS)
	$(CC) -o $@ -L/$(CURDIR)/ -lglob -o $(TARGET)
$(LIB): $(LIBOBJS)
	$(CC) -shared -Wl,-soname,$@ -o $@ $(LIBOBJS)
$(OTHERLIB): $(LIBOBJS)
	ar cr $(OTHERLIB) $(LIBOBJS)
	
oss.o : oss.c 
	$(CC) -fpic -c oss.c
prime.o : prime.c 
	$(CC) -fpic -c prime.c
.c.o:
	$(CC) $(CFLAGS) -c $<

.PHONY: clean
clean:
	rm -f *.o *~ $(LIB) $(TARGET) $(OBJS) $(OTHERLIB)

