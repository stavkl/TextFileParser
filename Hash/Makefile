FLAGS = -g -ansi -pedantic -Wall -Wconversion

SRC = GenericHash_test.c GenericHash_func.c 

INC_PATH = ../include

SRC__LIST_PATH = ../LinkedList/

SRC_LIST = ../LinkedList/genericDblList_funcOnItr.c ../LinkedList/genericDblList_func.c 

a.out: $(SRC) 
	gcc $(FLAGS) -I $(INC_PATH) $(SRC) $(SRC_LIST) -lm

run:
	./a.out

clean:
	rm a.out

