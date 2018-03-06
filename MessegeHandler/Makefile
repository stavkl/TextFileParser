FLAGS = -g -pedantic -Wall -Wconversion

SRC = manager.c messageHandler.c

INC_PATH = ../include

SRC_LIST = ../LinkedList/genericDblList_funcOnItr.c ../LinkedList/genericDblList_func.c 
SRC_HASH = ../Hash/GenericHash_func.c
SRC_MSNGR = ../Messenger/messenger.c

msgHandler: $(SRC) 
	gcc $(FLAGS) -I $(INC_PATH) $(SRC) $(SRC_LIST) $(SRC_HASH) $(SRC_MSNGR) -lpthread -o msgHandler

run:
	./a.out

clean:
	rm a.out

