CC = cc
CFLAGS = -Wall -Wextra -std=c11
LDLIBS = -lsqlite3

TARGET = shoe_tracker
SRCS = main.c db.c
OBJS = $(SRCS:.c=.o)

GUI_TARGET = shoe_gui
GUI_CFLAGS = $(CFLAGS) $(shell pkg-config --cflags raylib)
GUI_LDLIBS = $(shell pkg-config --libs raylib)

.PHONY: all clean gui

all: $(TARGET)

gui: $(GUI_TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

$(GUI_TARGET): gui.c
	$(CC) $(GUI_CFLAGS) -o $@ $< $(GUI_LDLIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET) $(GUI_TARGET)
