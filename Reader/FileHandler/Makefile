FLAGS = -g -pedantic -Wall -Wconversion

SRC = fileHandler.c

INC_PATH = ../../include

SRC_MSGR = ../../Messenger
MSGR = messenger.c

a.out: $(SRC) 
	gcc $(FLAGS) -I $(INC_PATH) $(SRC) $(SRC_MSGR)/$(MSGR) -o fileHandler

run:
	./a.out

clean:
	rm a.out
