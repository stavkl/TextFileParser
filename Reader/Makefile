FLAGS = -g -pedantic -Wall -Wconversion

SRC = reader.c

INC_PATH = ../include
MSGR_PATH = ../Messenger/messenger.c

a.out: $(SRC) 
	gcc $(FLAGS) -I $(INC_PATH) $(SRC) $(MSGR_PATH) -o reader

run:
	./a.out

clean:
	rm a.out

