CC = gcc
CFLAGS = -std=gnu++98 -Wall -lm -lX11 `pkg-config --cflags gtk+-3.0 libwnck-3.0`
LDFLAGS = `pkg-config --libs gtk+-3.0 libwnck-3.0`
DEPS = dock_icon.h ericdock.h clock.h drawing.h eric_window.h pager_item.h tooltip_window.h
OBJ = dock_icon.o tooltip_window.o eric_window.o clock.o main.o pager_item.o drawing.o

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

ericdock: $(OBJ)
	gcc $(LDFLAGS) $(CFLAGS) -o $@ $^
