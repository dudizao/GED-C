CC      = gcc
CFLAGS  = -Wall -Wextra -std=c99
TARGET  = ged
SRCS    = main.c documento.c arquivo.c
OBJS    = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c documento.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
