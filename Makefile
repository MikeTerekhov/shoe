CC = cc
CFLAGS = -Wall -Wextra -std=c11
LDLIBS = -lsqlite3

TARGET = shoe_tracker
SRCS = main.c db.c
OBJS = $(SRCS:.c=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
