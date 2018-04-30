CFLAGS= -Wall -Werror -g
LDFLAGS=-pthread -lm
CC=gcc
#arm-linux-gnueabihf-gcc
OBJECTS=test.o gps_decoder.o gps_calc.o
TARGET=5013P1

all: $(TARGET)
	
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

clean:
	rm ./$(TARGET) *.o
