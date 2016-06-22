ericwindow-test: main.c
	gcc -o ericwindow-test `pkg-config --cflags --libs gtk+-3.0` main.c
