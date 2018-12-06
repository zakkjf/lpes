CFLAGS= -Wall -Werror -g
LDFLAGS=-pthread -lm
CC=gcc
OBJECTS=main.o
TARGET=5013HW4P2C

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

clean:
	rm ./$(TARGET) *.o
