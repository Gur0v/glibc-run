CC = gcc
CFLAGS = -Wall -Wextra -O3 -I.
LDFLAGS =
TARGET = grun

all: $(TARGET)

$(TARGET): main.o
	$(CC) $(CFLAGS) -o $(TARGET) main.o $(LDFLAGS)

main.o: main.c config.h
	$(CC) $(CFLAGS) -c main.c

clean:
	rm -f *.o $(TARGET)

.PHONY: all clean

