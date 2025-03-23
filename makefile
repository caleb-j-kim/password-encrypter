# Define compiler
CC=g++

# Define compiler options
CFLAGS=-Wall -g

# Define the target executables
all: logger encryption driver

logger: logger.cpp
	$(CC) $(CFLAGS) logger.cpp -o logger

encryption: encryption.cpp
	$(CC) $(CFLAGS) encryption.cpp -o encryption

driver: driver.cpp
	$(CC) $(CFLAGS) driver.cpp -o driver

# Define clean command
clean:
	rm -f logger encryption driver