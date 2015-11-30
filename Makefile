CC = gcc
CFLAGS = `pkg-config --cflags --libs fuse`
WARN = -Wall -W
#File
SOURCES = fs.c
OBJECTS = fs.o
TARGET = fs


# build rule
.PHONY : all clean
all :
	$(CC) $(WARN) $(CFLAGS) $(SOURCES) -o $(TARGET) -lfuse

clean :
	-rm -f $(OBJECTS) $(TARGET)
