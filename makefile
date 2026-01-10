CC = gcc
# in case we add more driver files in the future
# CFLAGS specifies the compilation flags for the compiler.
# These flags can include options for optimization, debugging, and warning control.
CFLAGS = -I./include -I./FreeRTOS/include -I./drivers -Wall

# SRC specifies the list of source files for the project.
# These files are typically written in a programming language like C or C++ 
# and will be compiled into object files or directly into the final executable.
# Modify this variable to include all the source files required for the build process.
SRC = src/processing.c \
      src/monitor.c \
      src/communication.c \
      src/crc.c \
      src/main.c \
      drivers/hardware_uart.c

OBJ = $(SRC:.c=.o)
TARGET = qrng_system

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	rm -f src/*.o drivers/*.o *.o $(TARGET)