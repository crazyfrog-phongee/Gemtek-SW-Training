TITLE_COLOR = \033[33m
NO_COLOR = \033[0m
FLAGS = -Wall -std=c99 -fdiagnostics-color=auto -lm -lpthread -L/usr/lib -lssl -lcrypto
CC := gcc

DEBUG_LVL = 0
SPEEDTEST_CONFIG = -DDEBUG_LVL=$(DEBUG_LVL) -DUSE_HTTPS=$(USE_HTTPS)
USE_HTTPS = 1

INC = ./inc
SRC = ./src/*.c

# when excuting make, compile all exe's
all: clean 
# When trying to compile one of the executables, first look for its .c files
# Then check if the libraries are in the lib folder
	@echo "$(TITLE_COLOR)\n***** CPPCHECK *****$(NO_COLOR)"
	cppcheck --enable=all --suppress=missingIncludeSystem main.c $(SRC)
	@echo "$(TITLE_COLOR)\n***** COMPILING *****$(NO_COLOR)"
	$(CC) -I $(INC) main.c $(SRC) $(FLAGS) $(SPEEDTEST_CONFIG) -g -o ./bin/exec_file

# do not look for files called clean, clean-all or this will be always a target
.PHONY: clean
clean:
	rm -rf ./bin/exec_file
	sudo rm -rf /tmp/speedtest-config.php
	sudo rm -rf /tmp/speedtest-servers-static.php?

run:
	./bin/exec_file

leak: all
	@echo "$(TITLE_COLOR)\n***** LEAK CHECK *****$(NO_COLOR)"
	valgrind --leak-check=full -v --track-origins=yes --show-leak-kinds=all --read-var-info=yes ./bin/exec_file