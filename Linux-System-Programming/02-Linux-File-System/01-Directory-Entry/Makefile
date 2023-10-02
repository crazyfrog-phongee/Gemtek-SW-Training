TITLE_COLOR = \033[33m
NO_COLOR = \033[0m
FLAGS = -Wall -std=c99 -fdiagnostics-color=auto
CC := gcc

all:
	$(CC) -g -o ./bin/exec_file main.c $(FLAGS)

clean:
	rm -rf ./bin/exec_file

run:
	./bin/exec_file