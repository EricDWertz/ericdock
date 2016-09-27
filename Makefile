ericdock: main.c
	gcc -o ericdock -lm -lX11 `pkg-config --cflags --libs gtk+-3.0 libwnck-3.0` main.c
