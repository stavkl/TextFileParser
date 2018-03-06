#include "list_itr.h"
#include "list.h"
#include "list_functions.h"
#include <stdlib.h>
#include <stdio.h>
#define MAGIC 0xF00DBABE
#define EMPTY 1
#define NOT_EMPTY 0
#define EQUAL 1
#define NOT_EQUAL 0

#define IS_HEAD(N) ((N)->m_prev == (N))
#define IS_TAIL(N) ((N)->m_next == (N))

#define IS_A_LIST(L) ((L) && MAGIC == (L)->magic)

#define LIST_HEAD(L) (&(L)->m_head)
#define LIST_TAIL(L) (&(L)->m_tail)

#define LIST_FIRST(L) ((L)->m_head.m_next)
#define LIST_LAST(L) ((L)->m_tail.m_prev)

typedef struct Node Node;

struct Node{
	void* 	m_data;
	Node* 	m_next;
	Node* 	m_prev;
};

struct List{
	size_t 	magic;
	Node 	m_head;
	Node 	m_tail;
};

/*-----------------------Helper Functions Declerations-------------------------------*/
static List_Result ListPopper(List* _list, void** _data, Node* _elementToRemove);
static List_Result ListPusher(void* _data, Node* left, Node* right);
static void Remove_Element(List* _list, _elementDestroy Destroyer);
size_t GetMagic(List* _list);
/*-----------------------------------------------------------------------------------*/

/*---------------------------------DLL Create----------------------------------------*/
List* ListCreate(void){
	List* list = (List*)malloc(sizeof(List));
	if (list == NULL){
		list = NULL;
	}
	
	list->m_head.m_prev = &(list->m_head);
	list->m_head.m_next = &(list->m_tail);

	list->m_tail.m_prev = &(list->m_head);
	list->m_tail.m_next = &(list->m_tail);
	
	list->magic = MAGIC;

	return list;
}

/*--------------------------------DLL Destroy----------------------------------------*/
void ListDestroy(List** _list, _elementDestroy Destroyer){

	if(_list == NULL || !IS_A_LIST(*_list)){
		return;
	}

	Remove_Element(*_list, Destroyer);

	(*_list)->magic = 0;
	free(*_list);
	*_list = NULL;
}

/*---------------------------------Push Head-----------------------------------------*/
List_Result ListPushHead(List* _list, void* _data){
	if(_list == NULL)
	{
		return LIST_UNINITIALIZED_ERROR;
	}
    
	return	ListPusher(_data, &_list->m_head, _list->m_head.m_next);
}

/*---------------------------------Push Tail-----------------------------------------*/
List_Result ListPushTail(List* _list, void* _data){
	if(_list == NULL)
	{
		return LIST_UNINITIALIZED_ERROR;
	}
    
	return ListPusher(_data, _list->m_tail.m_prev, &_list->m_tail);
}

/*----------------------------------Pop Head-----------------------------------------*/
List_Result ListPopHead(List* _list, void** _data){
    
	if(_list == NULL)
	{
		return LIST_UNINITIALIZED_ERROR;
	}
	return ListPopper(_list, _data, _list->m_head.m_next);
}

/*----------------------------------Pop Tail-----------------------------------------*/
List_Result ListPopTail(List* _list, void** _data){
    
	if(_list == NULL)
	{
		return LIST_UNINITIALIZED_ERROR;
	}
    
	return ListPopper(_list, _data, _list->m_tail.m_prev);
}

/*------------------------------DLL Count Items--------------------------------------*/
size_t ListSize(const List* _list){
    
	size_t counter;
	Node* current;
	
	if(_list == NULL)
	{
		return 0;
	}
	
	counter = 0;
	current = _list->m_head.m_next;
	
	while(current != &_list->m_tail)
	{
		++counter;
		current = current->m_next;
	}

	return counter;
}

/*------------------------------Is Empty List----------------------------------------*/
int ListIsEmpty(List* _list){
	
    if(_list == NULL || _list->m_head.m_next == &_list->m_tail)
	{
		return EMPTY;
	}
	return NOT_EMPTY;
}


/*---------------------------------DLL Getters---------------------------------------*/
size_t GetMagic(List* _list){
	if(_list == NULL){
		return 0;
	}
	return _list->magic;
}

/*------------------------------Helper Functions-------------------------------------*/

static List_Result ListPopper(List* _list, void** _data, Node* _elementToRemove){
/*helper function for the logic of head/tail pop functions
disconnects an element and connects its next and prev properties to each other*/
	
	if(_list == NULL || _data == NULL || _elementToRemove == NULL)
	{
		return LIST_UNINITIALIZED_ERROR;
	}
	
	/* Empty list */
	if(_list->m_head.m_next == &_list->m_tail)
	{
		return LIST_UNDERFLOW;
	}

	*_data = _elementToRemove->m_data;

	_elementToRemove->m_next->m_prev = _elementToRemove->m_prev;
	_elementToRemove->m_prev->m_next = _elementToRemove->m_next;

	free(_elementToRemove);
	
	return LIST_SUCCESS;
}

static List_Result ListPusher(void* _data, Node* left, Node* right){
/*helper function for the logic of pushHeah and pushTail functions
It creates a new node and connects it according to the left/ right coordinates */

	Node* newNode;
	
	if(left == NULL || right == NULL)
	{
		return LIST_UNINITIALIZED_ERROR;
	}
	
	/* List is empty */	
	newNode = malloc(sizeof(Node));
	if(newNode == NULL)
	{
		return LIST_UNINITIALIZED_ERROR;
	}
	
	newNode->m_data = _data;	

	newNode->m_next = right;
	newNode->m_prev = left;

	right->m_prev = newNode;
	left->m_next = newNode;

	return LIST_SUCCESS;
}

static void Remove_Element(List* _list, _elementDestroy Destroyer){
	Node* current;
	Node* tmp;

	current = _list->m_head.m_next;
	while(current != &(_list->m_tail))
	{
		tmp = current->m_next;
		if(Destroyer != NULL){
			Destroyer(current);
		}

		else{
			free(current);
		}

		current = tmp;
	}
}


