FLAGS = -g -ansi -pedantic -Wall -Wconversion
SRC = genericDblList_func.c genericDblList_funcOnItr.c genericDblList_test.c
INC_PATH = ../include

a.out: $(SRC) 
	gcc $(FLAGS) -I$(INC_PATH) $(SRC)
run:
	./a.out
clean:
	rm a.out 

