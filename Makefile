ericwindow-test: main.c
	gcc -o ericwindow-test -lX11 `pkg-config --cflags --libs gtk+-3.0 libwnck-3.0` main.c
