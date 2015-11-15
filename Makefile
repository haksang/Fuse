CC = gcc
CFLAGS = -Wall -W -D_FILE_OFFSET_BITS=64

#File
SOURCES = hello.c
OBJECTS = hello.o
TARGET = hello


# build rule
.PHONY : all clean
all :
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET) -lfuse

clean :
	-rf -f $(OBJECTS) $(TARGET)
