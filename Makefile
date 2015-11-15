CC = gcc
CFLAGS = -Wall -W -D_FILE_OFFSET_BITS=64

#File
SOURCES = fs.c
OBJECTS = fs.o
TARGET = fs


# build rule
.PHONY : all clean
all :
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET) -lfuse

clean :
	-rm -f $(OBJECTS) $(TARGET)
