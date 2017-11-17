CC = gcc
CFLAGS = -Wunused-result -g -Wall -lm -lX11 -lXext `pkg-config --cflags gtk+-3.0 libwnck-3.0`
LDFLAGS = `pkg-config --libs gtk+-3.0 libwnck-3.0`
DEPS = dock_icon.h ericdock.h clock.h drawing.h eric_window.h pager_item.h tooltip_window.h control.h xutils.h
OBJ = dock_icon.o tooltip_window.o eric_window.o clock.o main.o pager_item.o drawing.o control.o xutils.o

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

ericdock: $(OBJ)
	gcc $(LDFLAGS) $(CFLAGS) -o $@ $^
