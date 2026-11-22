
CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -pthread
TARGET = minios
SRC = src/main.c src/sched/parser.c src/sched/simulator.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all clean

