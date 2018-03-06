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

#define IS_A_LIST(L) ((L)&& MAGIC == (L)->magic))

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
/*-------------------------------ListItr Sort----------------------------------------*/
/*
void ListItr_Sort(ListItr _begin, ListItr _end, LessFunction _less){
	void* temp;
	int swapCounter = 1;

	while(_begin != _end && ListItr_Next(_begin) != _end && swapCounter != 0){
		swapCounter = 0;
		if(_less(ListItr_Next(_begin),_begin) == 1){
			temp = ListItr_Next(_begin);
			ListItr_Next(_begin) = _begin;
			_begin = temp;
			swapCounter = 1;
		}
		_begin = ListItr_Next(_begin);
	}
}
*/
/*-------------------------------ListItr Merge---------------------------------------*/
/*
ListItr ListItr_Merge(	ListItr _destBegin, ListItr _firstBegin, ListItr _firstEnd,
						ListItr _secondBegin, ListItr _secondEnd, LessFunction _less){

	ListItr_Splice(_destBegin, _firstBegin, _firstEnd);
	ListItr_Splice(_destBegin, _secondBegin, _secondEnd);

	if(_less != NULL){
		ListItr_Sort(_firstBegin, _destBegin, _less);
	}
	return _destBegin;
*/
/*-------------------------------ListItr Cut-----------------------------------------*/
List* ListItr_Cut(ListItr _begin, ListItr _end){

	List* newList = ListCreate();
	if(newList == NULL){
		return NULL;
	}

	ListItr_Splice(ListItr_End(newList), _begin, _end);

	return newList;
}

/*-------------------------------ListItr Unique--------------------------------------*/
List* ListItr_Unique(ListItr _begin, ListItr _end, EqualsFunction _equals){

	void* removedData;
	List* newList = ListCreate();
	if(newList == NULL){
		return NULL;
	}

	while(_begin != _end){
		if(ListItr_Next(_begin) == _end){
			break;
		}

		if(_equals(_begin, ListItr_Next(_begin)) == EQUAL){
			removedData = ListItr_Remove(ListItr_Next(_begin));
			ListPushTail(newList, removedData);	
			
		}
		else{
			_begin = ListItr_Next(_begin);
		}

	}
	
	return newList;

}
/*-------------------------------ListItr Splice--------------------------------------*/
ListItr ListItr_Splice(ListItr _dest, ListItr _begin, ListItr _end){
	ListItr tempItr;
	
	while(_begin != _end){
		tempItr = ListItr_Next(_begin);

		ListItr_InsertBefore(_dest, ListItr_Remove(_begin));
		_begin = tempItr;
	}
	return _begin;
}

/*-------------------------------ListItr Count If------------------------------------*/
size_t ListItr_CountIf(ListItr _begin, ListItr _end, PredicateFunction _predicate, void* _context){
	size_t counter = 0;

	if(!_predicate){
		return 0;
	}

	while(_begin != _end){
		if(_predicate(_begin, _context)){
			++counter;
		}
		_begin = ListItr_Next(_begin);
	}
	return counter;
}

/*-------------------------------ListItr For Each------------------------------------*/
ListItr ListItr_ForEach(ListItr _begin, ListItr _end, ListActionFunction _action, void* _context){
	if(!_action){
		return NULL;
	}

	while(_begin != _end){

		if(!_action(_begin, _context)){
			return _begin;
		}
		_begin = ListItr_Next(_begin);
	}
	return _end;
}

/*-------------------------------ListItr Find First----------------------------------*/
ListItr ListItr_FindFirst(ListItr _begin, ListItr _end, PredicateFunction _predicate, void* _context){
	/*Assuming that _begin and _end are in the same list and are in their respective order*/
	if(!_predicate){
		return _end;
	}
	while(_begin != _end){
		if(_predicate(_begin, _context)){
			return _begin;
		}		
		_begin = ListItr_Next(_begin);
	}
	return _end;
}

/*-------------------------------ListItr_Begin---------------------------------------*/
ListItr ListItr_Begin(const List* _list){
	if(_list == NULL){
		return NULL;
	}
	
	return (void*)(_list->m_head.m_next);
}

/*---------------------------------ListItr_End---------------------------------------*/
ListItr ListItr_End(const List* _list){
	if(_list == NULL){
		return NULL;
	}
	
	return (void*)(&(_list->m_tail));
}

/*-------------------------------ListItr_Equals--------------------------------------*/
int ListItr_Equals(const ListItr _a, const ListItr _b){
	if(_a == _b){
		return EQUAL;
	}
	else{
		return NOT_EQUAL;
	}
}

/*---------------------------------ListItr_Next--------------------------------------*/
ListItr ListItr_Next(ListItr _itr){
	if(_itr == NULL){
		return NULL;
	}
	return (void*)(((Node*)(_itr))->m_next);
}

/*-------------------------------ListItr_Prev----------------------------------------*/
ListItr ListItr_Prev(ListItr _itr){
	if(_itr == NULL){
		return NULL;
	}
	return (void*)(((Node*)(_itr))->m_prev);
}

/*---------------------------------ListItr_Get---------------------------------------*/
void* ListItr_Get(ListItr _itr){
	if(_itr == NULL){
		return NULL;
	}
	return (((Node*)_itr)->m_data);
}

/*--------------------------------ListItr_Set----------------------------------------*/
void* ListItr_Set(ListItr _itr, void* _element){
	void* prevData = (void*)(((Node*)(_itr))->m_data);
	(((Node*)_itr)->m_data) = _element;
	return prevData;
}

/*------------------------------ListItr_InsertBefore---------------------------------*/
ListItr ListItr_InsertBefore(ListItr _itr, void* _element){
	Node* newNode;
/**/
	if(_itr == NULL || _element == NULL || ((Node*)_itr)->m_prev == ((Node*)_itr)->m_prev->m_prev){
		return NULL;
	}

	newNode = malloc(sizeof(Node));
	if(newNode == NULL)
	{
		return NULL;
	}
	
	newNode->m_data = _element;	

	newNode->m_next = (Node*)_itr;
	newNode->m_prev = ((Node*)_itr)->m_prev;

	((Node*)_itr)->m_prev->m_next = newNode;
	((Node*)_itr)->m_prev= newNode;
	
	return ((void*)newNode);
}
/*-------------------------------ListItr_Remove--------------------------------------*/
void* ListItr_Remove(ListItr _itr){
	void* removedData = NULL;
	
	if(	_itr == NULL || ListItr_Next(_itr) == _itr || ListItr_Prev(_itr) == _itr){
		return NULL;
	}
	removedData = ((Node*)_itr)->m_data;

	((Node*)_itr)->m_next->m_prev = ((Node*)_itr)->m_prev;
	((Node*)_itr)->m_prev->m_next =((Node*)_itr)->m_next;

	((Node*)_itr)->m_prev = NULL;
	((Node*)_itr)->m_next = NULL;

	free(_itr);

	return removedData;
}
