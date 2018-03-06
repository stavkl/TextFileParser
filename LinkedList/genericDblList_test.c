#include "mu_test.h"
#include "list_itr.h"
#include "list_functions.h"
#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#define EQUAL 1
#define NOT_EQUAL 0

/*-----------Helper Functions Declerations------------*/
size_t GetMagic(List* _list);
int PrintInt(void* _element, void* _context);
int FindBiggerThanContext(void* _element, void* _context);
int AreIntsEqual(void* _elementA, void* _elementB);
/*-------------------Create Tests --------------------*/
UNIT(List_Create_Normal)
	List* ls = ListCreate();
	ASSERT_THAT(ls != NULL);
	ASSERT_THAT(GetMagic(ls) == 0xF00DBABE);
	ListDestroy(&ls, NULL);
END_UNIT

/*-------------------Destroy Tests -------------------*/
UNIT(List_Destroy_Null)
	List* ls = ListCreate();
	ASSERT_THAT(ls != NULL);
	ASSERT_THAT(GetMagic(ls) == 0xF00DBABE);
	ListDestroy(&ls, NULL);
END_UNIT

UNIT(List_Destroy_DoubleFree)
	List* ls = ListCreate();
	ListDestroy(&ls, NULL);
	ListDestroy(&ls, NULL);
	ASSERT_THAT(1);
END_UNIT

/*-------------------Push Head Tests -----------------*/
UNIT(List_Push_head_Null)
	int item = 5;
	void* pItem = &item;
	ASSERT_THAT(ListPushHead(NULL, pItem) == LIST_UNINITIALIZED_ERROR);
END_UNIT

UNIT(List_Push_head_One)
	int item = 5;
	void* pItem = &item;
	List* ls = ListCreate();
	ASSERT_THAT(ListPushHead(ls, pItem) == LIST_SUCCESS);
	ListDestroy(&ls, NULL);
END_UNIT

UNIT(List_Push_head_Many)
	int item1 = 1;
	int item2 = 2;
	int item3 = 3;
	int item4 = 4;
	void* pItem1 = &item1;
	void* pItem2 = &item2;
	void* pItem3 = &item3;
	void* pItem4 = &item4;
	List* ls = ListCreate();
	ASSERT_THAT(ListPushHead(ls, pItem1) == LIST_SUCCESS);
	ASSERT_THAT(ListPushHead(ls, pItem2) == LIST_SUCCESS);
	ASSERT_THAT(ListPushHead(ls, pItem3) == LIST_SUCCESS);
	ASSERT_THAT(ListPushHead(ls, pItem4) == LIST_SUCCESS);
	ListDestroy(&ls, NULL);
END_UNIT

UNIT(List_Push_head_One_Then_Pop)
	int item = 5;
	void* pItem = &item;
	void* receivedItem;
	List* ls = ListCreate();
	ASSERT_THAT(ListPushHead(ls, pItem) == LIST_SUCCESS);
	ListPopHead(ls, &receivedItem);
	ASSERT_THAT(*(int*)receivedItem == 5);
	ListDestroy(&ls, NULL);
END_UNIT

UNIT(List_Push_head_Many_Then_Pop)
	int item1 = 1;
	int item2 = 2;
	int item3 = 3;
	int item4 = 4;
	void* pItem1 = &item1;
	void* pItem2 = &item2;
	void* pItem3 = &item3;
	void* pItem4 = &item4;
	void* receivedItem;

	List* ls = ListCreate();
	ASSERT_THAT(ListPushHead(ls, pItem1) == LIST_SUCCESS);
	ASSERT_THAT(ListPushHead(ls, pItem2) == LIST_SUCCESS);
	ASSERT_THAT(ListPushHead(ls, pItem3) == LIST_SUCCESS);
	ASSERT_THAT(ListPushHead(ls, pItem4) == LIST_SUCCESS);
	ListPopHead(ls, &receivedItem);
	ASSERT_THAT(*(int*)receivedItem == 4);
	ListPopHead(ls, &receivedItem);
	ASSERT_THAT(*(int*)receivedItem == 3);
	ListPopHead(ls, &receivedItem);
	ASSERT_THAT(*(int*)receivedItem == 2);
	ListPopHead(ls, &receivedItem);
	ASSERT_THAT(*(int*)receivedItem == 1);
	ListDestroy(&ls, NULL);
END_UNIT
/*-------------------Pop Head Tests ------------------*/
UNIT(List_Pop_head_Null)
	int item = 5;
	void* pItem = &item;
	ASSERT_THAT(ListPopHead(NULL, pItem) == LIST_UNINITIALIZED_ERROR);
END_UNIT

UNIT(List_Pop_head_From_Empty)
	void* receivedItem;
	List* ls = ListCreate();
	ASSERT_THAT(ListPopHead(ls, &receivedItem) == LIST_UNDERFLOW);
	ListDestroy(&ls, NULL);
END_UNIT

UNIT(List_Pop_Too_Many_Heads)
	int item1 = 1;
	int item2 = 2;
	int item3 = 3;
	int item4 = 4;
	void* pItem1 = &item1;
	void* pItem2 = &item2;
	void* pItem3 = &item3;
	void* pItem4 = &item4;
	void* receivedItem;

	List* ls = ListCreate();
	ListPushHead(ls, pItem1);
	ListPushHead(ls, pItem2);
	ListPushHead(ls, pItem3);
	ListPushHead(ls, pItem4);
	ListPopHead(ls, &receivedItem);
	ListPopHead(ls, &receivedItem);
	ListPopHead(ls, &receivedItem);
	ListPopHead(ls, &receivedItem);

	ASSERT_THAT(ListPopHead(ls, &receivedItem) == LIST_UNDERFLOW);
	ListDestroy(&ls, NULL);
END_UNIT

UNIT(Itr_Begin_null)
	ASSERT_THAT(ListItr_Begin(NULL) == NULL);
END_UNIT

UNIT(Itr_Begin_OK)
	int item = 5;
	void* pItem = &item;
	int* data;
	ListItr receivedData;

	List* ls = ListCreate();
	ListPushHead(ls, pItem);

	receivedData = ListItr_Begin(ls);
	data = (int*)ListItr_Get(receivedData);
	ASSERT_THAT(*data == 5);
	ListDestroy(&ls, NULL);
END_UNIT

UNIT(Itr_End_null)
	ASSERT_THAT(ListItr_End(NULL) == NULL);
END_UNIT

UNIT(Itr_End_OK)
	int item = 5;
	void* pItem = &item;
	int* data;
	ListItr receivedEnd;
	ListItr receivedEndPrev;
	List* ls = ListCreate();
	ListPushHead(ls, pItem);

	receivedEnd = ListItr_End(ls);
	receivedEndPrev = ListItr_Prev(receivedEnd);
	data = (int*)ListItr_Get(receivedEndPrev);
	ASSERT_THAT(*data == 5);
	ListDestroy(&ls, NULL);
END_UNIT

UNIT(Itr_Equal_Yes)
	int item = 5;
	void* pItem = &item;

	ListItr receivedEnd;
	ListItr receivedEndPrev;

	ListItr receivedBegin;

	List* ls = ListCreate();
	ListPushHead(ls, pItem);

	receivedBegin = ListItr_Begin(ls);
	receivedEnd = ListItr_End(ls);

	receivedEndPrev = ListItr_Prev(receivedEnd);
	ASSERT_THAT(ListItr_Equals(receivedEndPrev, receivedBegin) == EQUAL);
	ListDestroy(&ls, NULL);
END_UNIT

UNIT(Itr_Equal_No)
	int item1 = 5;
	int item2 = 10;
	void* pItem1 = &item1;
	void* pItem2 = &item2;

	ListItr receivedEnd;
	ListItr receivedEndPrev;

	ListItr receivedBegin;
	List* ls = ListCreate();
	ListPushHead(ls, pItem1);
	ListPushHead(ls, pItem2);

	receivedBegin = ListItr_Begin(ls);
	receivedEnd = ListItr_End(ls);

	receivedEndPrev = ListItr_Prev(receivedEnd);
	ASSERT_THAT(ListItr_Equals(receivedEndPrev, receivedBegin) == NOT_EQUAL);
	ListDestroy(&ls, NULL);
END_UNIT

UNIT(Itr_Next_head)

	ListItr receivedEnd;
	ListItr receivedBegin;
	ListItr BeginNext;
	List* ls = ListCreate();

	receivedBegin = ListItr_Begin(ls);
	BeginNext = ListItr_Next(receivedBegin);
	receivedEnd = ListItr_End(ls);

	ASSERT_THAT(ListItr_Equals(BeginNext, receivedEnd) == EQUAL);
	ListDestroy(&ls, NULL);
END_UNIT

UNIT(Itr_Next_tail)

	ListItr receivedEnd;
	ListItr tailNext;
	List* ls = ListCreate();

	receivedEnd = ListItr_End(ls);
	tailNext = ListItr_Next(receivedEnd);

	ASSERT_THAT(ListItr_Equals(tailNext, receivedEnd) == EQUAL);
	ListDestroy(&ls, NULL);

END_UNIT

UNIT(Itr_Prev_tail)

	ListItr receivedBegin;
	ListItr BeginPrev;
	ListItr PrevNext;
	List* ls = ListCreate();

	receivedBegin = ListItr_Begin(ls);
	BeginPrev = ListItr_Prev(receivedBegin);
	PrevNext = ListItr_Next(BeginPrev);
	ASSERT_THAT(ListItr_Equals(PrevNext, receivedBegin) == EQUAL);
	ListDestroy(&ls, NULL);
END_UNIT

UNIT(Itr_Prev_head)

	ListItr receivedBegin;
	ListItr BeginPrev;
	ListItr PrevPrev;
	List* ls = ListCreate();

	receivedBegin = ListItr_Begin(ls);
	BeginPrev = ListItr_Prev(receivedBegin);
	PrevPrev = ListItr_Prev(BeginPrev);
	ASSERT_THAT(ListItr_Equals(PrevPrev, BeginPrev) == EQUAL);
	ListDestroy(&ls, NULL);

END_UNIT

UNIT(Itr_Set_normal)
	int item = 5;
	int item2 = 10;
	void* pItem = &item;
	void* pItem2 = &item2;
	void* receivedBegin;

	List* ls = ListCreate();
	ListPushHead(ls, pItem);

	receivedBegin = ListItr_Begin(ls);
	ListItr_Set(receivedBegin, pItem2);
	
	ASSERT_THAT(item2 = *(int*)ListItr_Get(receivedBegin));

	ListDestroy(&ls, NULL);
END_UNIT

UNIT(Itr_InsertBefore_tail)
	int item = 5;
	int item2 = 10;
	int* data;
	void* pItem = &item;
	void* pItem2 = &item2;
	void* receivedBegin;
	void* receivedEnd;
	void* BeginNext;

	List* ls = ListCreate();
	ListPushHead(ls, pItem);

	receivedEnd = ListItr_End(ls);
	ListItr_InsertBefore(receivedEnd, pItem2);
	
	receivedBegin = ListItr_Begin(ls);
	BeginNext = ListItr_Next(receivedBegin);
	data =(int*)ListItr_Get(BeginNext);
	ASSERT_THAT(item2 = *data);

	ListDestroy(&ls, NULL);
END_UNIT

UNIT(Itr_InsertBefore_Normal)
	int item = 5;
	int item2 = 10;
	int item3 = 7;
	int* data;
	void* pItem = &item;
	void* pItem2 = &item2;
	void* pItem3 = &item3;
	void* receivedFirst;
	void* receivedSecond;
	void* receivedNewSecond;

	List* ls = ListCreate();
	ListPushHead(ls, pItem);
	ListPushHead(ls, pItem2);

	receivedFirst = ListItr_Begin(ls);
	receivedSecond = ListItr_Next(receivedFirst);
	ListItr_InsertBefore(receivedSecond, pItem3);
	
	receivedNewSecond = ListItr_Next(receivedFirst);
	data =(int*)ListItr_Get(receivedNewSecond);
	ASSERT_THAT(item3 = *data);

	ListDestroy(&ls, NULL);
END_UNIT

UNIT(Itr_Remove_Null)
	ASSERT_THAT(ListItr_Remove(NULL) == NULL);
END_UNIT

UNIT(Itr_Remove_normal)
	int item = 5;
	int item2 = 10;
	int item3 = 7;
	int* data;
	void* pItem = &item;
	void* pItem2 = &item2;
	void* pItem3 = &item3;
	void* receivedFirst;
	void* receivedSecond;
	void* receivedNewSecond;

	List* ls = ListCreate();
	ListPushHead(ls, pItem);
	ListPushHead(ls, pItem2);

	receivedFirst = ListItr_Begin(ls);
	receivedSecond = ListItr_Next(receivedFirst);
	receivedNewSecond = ListItr_InsertBefore(receivedSecond, pItem3);
	
	data = ListItr_Remove(receivedNewSecond);
	ASSERT_THAT(item3 = *data);

	ListDestroy(&ls, NULL);
END_UNIT

UNIT(Func_Itr_For_Each_Normal)
	int item = 5;
	int item2 = 10;
	int item3 = 7;
	void* result;
	void* pItem = &item;
	void* pItem2 = &item2;
	void* pItem3 = &item3;
	void* receivedFirst;
	void* receivedLast;

	List* ls = ListCreate();
	ListPushHead(ls, pItem);
	ListPushHead(ls, pItem2);
	ListPushHead(ls, pItem3);

	receivedFirst = ListItr_Begin(ls);
	receivedLast = ListItr_End(ls);
	result = ListItr_ForEach(receivedFirst, receivedLast, PrintInt, NULL);
	ASSERT_THAT( result == receivedLast);
	ListDestroy(&ls, NULL);
END_UNIT

UNIT(Func_Itr_For_Each_Null)
	int item = 5;
	int item2 = 10;
	int item3 = 7;

	void* pItem = &item;
	void* pItem2 = &item2;
	void* pItem3 = &item3;
	void* receivedFirst;
	void* receivedLast;

	List* ls = ListCreate();
	ListPushHead(ls, pItem);
	ListPushHead(ls, pItem2);
	ListPushHead(ls, pItem3);

	receivedFirst = ListItr_Begin(ls);
	receivedLast = ListItr_End(ls);

	ASSERT_THAT(ListItr_ForEach(receivedFirst, receivedLast, NULL, NULL) == NULL);
	ListDestroy(&ls, NULL);
END_UNIT

UNIT(Func_Itr_FindFirst_Null)
	int item = 5;
	int item2 = 10;
	int item3 = 7;

	void* pItem = &item;
	void* pItem2 = &item2;
	void* pItem3 = &item3;
	void* receivedFirst;
	void* receivedLast;

	List* ls = ListCreate();
	ListPushHead(ls, pItem);
	ListPushHead(ls, pItem2);
	ListPushHead(ls, pItem3);

	receivedFirst = ListItr_Begin(ls);
	receivedLast = ListItr_End(ls);

	ASSERT_THAT(ListItr_FindFirst(receivedFirst, receivedLast, NULL, NULL) == receivedLast);
	ListDestroy(&ls, NULL);
END_UNIT

UNIT(Func_Itr_FindFirst_Normal)
	int item = 5;
	int item2 = 10;
	int item3 = 7;
	int* FinalResult;
	void* data = &item3;
	void* pItem = &item;
	void* pItem2 = &item2;
	void* pItem3 = &item3;
	void* receivedFirst;
	void* receivedLast;
	void* result;
	
	List* ls = ListCreate();
	ListPushHead(ls, pItem);
	ListPushHead(ls, pItem2);
	ListPushHead(ls, pItem3);

	receivedFirst = ListItr_Begin(ls);
	receivedLast = ListItr_End(ls);

	result = ListItr_FindFirst(receivedFirst, receivedLast, FindBiggerThanContext, data);
	FinalResult = ListItr_Get(result);
	ASSERT_THAT(10 == *(int*)FinalResult);
	ListDestroy(&ls, NULL);
END_UNIT

UNIT(Func_Itr_CountIf_Null)
	int item = 5;
	int item2 = 10;
	int item3 = 7;

	void* pItem = &item;
	void* pItem2 = &item2;
	void* pItem3 = &item3;
	void* receivedFirst;
	void* receivedLast;

	List* ls = ListCreate();
	ListPushHead(ls, pItem);
	ListPushHead(ls, pItem2);
	ListPushHead(ls, pItem3);

	receivedFirst = ListItr_Begin(ls);
	receivedLast = ListItr_End(ls);

	ASSERT_THAT(ListItr_CountIf(receivedFirst, receivedLast, NULL, NULL) == 0);
	ListDestroy(&ls, NULL);
END_UNIT

UNIT(Func_Itr_CountIf_Normal)
	size_t result = 0;
	int item1 = 6;
	int item2 = 5;
	int item3 = 4;
	int item4 = 3;
	int item5 = 2;
	int item6 = 1;
	
	int context = 2;
	void* data = &context;
	void* receivedFirst;
	void* receivedLast;

	List* ls = ListCreate();
	ListPushHead(ls, &item1);
	ListPushHead(ls, &item2);
	ListPushHead(ls, &item3);
	ListPushHead(ls, &item4);
	ListPushHead(ls, &item5);
	ListPushHead(ls, &item6);

	receivedFirst = ListItr_Begin(ls);
	receivedLast = ListItr_End(ls);

	result = ListItr_CountIf(receivedFirst, receivedLast, FindBiggerThanContext, data);
	ASSERT_THAT(4 == result);
	ListDestroy(&ls, NULL);
END_UNIT

UNIT(Func_Itr_Splice_Normal)

	int item1 = 6;
	int item2 = 5;
	int item3 = 4;
	int item4 = 3;
	int item5 = 2;
	int item6 = 1;
	
	void* receivedFirst;
	void* receivedSecond;
	void* receivedThird;
	void* receivedBeforeLast;
	void* receivedLast;
	void* SpliceRes;

	List* ls = ListCreate();
	ListPushHead(ls, &item1);
	ListPushHead(ls, &item2);
	ListPushHead(ls, &item3);
	ListPushHead(ls, &item4);
	ListPushHead(ls, &item5);
	ListPushHead(ls, &item6);

	receivedFirst = ListItr_Begin(ls);
	receivedSecond = ListItr_Next(receivedFirst);
	receivedThird = ListItr_Next(receivedSecond);

	receivedLast = ListItr_End(ls);
	receivedBeforeLast = ListItr_Prev(receivedLast);

	ListItr_ForEach(receivedFirst, receivedLast, PrintInt, NULL);

	SpliceRes = ListItr_Splice(receivedBeforeLast, receivedFirst, receivedThird);
	/*printf("After Splice:\n");*/

	receivedFirst = ListItr_Begin(ls);
	receivedLast = ListItr_End(ls);
	ListItr_ForEach(receivedFirst, receivedLast, PrintInt, NULL);

	ASSERT_THAT(SpliceRes == receivedFirst);
	ListDestroy(&ls, NULL);
END_UNIT

UNIT(Func_Itr_Unique_Normal)

	List* newList;
	int item1 = 6;
	int item2 = 6;
	int item3 = 5;
	int item4 = 4;
	int item5 = 4;
	int item6 = 3;
	
	void* receivedFirst;
	void* receivedLast;

	List* ls = ListCreate();
	ListPushHead(ls, &item1);
	ListPushHead(ls, &item2);
	ListPushHead(ls, &item3);
	ListPushHead(ls, &item4);
	ListPushHead(ls, &item5);
	ListPushHead(ls, &item6);

	receivedFirst = ListItr_Begin(ls);
	receivedLast = ListItr_End(ls);

	/*printf("Old List Before Unique:\n");*/
	ListItr_ForEach(receivedFirst, receivedLast, PrintInt, NULL);

	newList = ListItr_Unique(receivedFirst, receivedLast, AreIntsEqual);
	/*printf("New List After Unique:\n");*/

	receivedFirst = ListItr_Begin(newList);
	receivedLast = ListItr_End(newList);
	ListItr_ForEach(receivedFirst, receivedLast, PrintInt, NULL);

	ListDestroy(&ls, NULL);
	ListDestroy(&newList, NULL);
END_UNIT

UNIT(Func_Itr_Cut_Normal)

	List* newList;
	int item1 = 6;
	int item2 = 5;
	int item3 = 4;
	int item4 = 3;
	int item5 = 2;
	int item6 = 1;
	
	void* receivedFirst;
	void* receivedSecond;
	void* receivedThird;

	List* ls = ListCreate();
	ListPushHead(ls, &item1);
	ListPushHead(ls, &item2);
	ListPushHead(ls, &item3);
	ListPushHead(ls, &item4);
	ListPushHead(ls, &item5);
	ListPushHead(ls, &item6);

	receivedFirst = ListItr_Begin(ls);
	receivedSecond = ListItr_Next(receivedFirst);
	receivedThird = ListItr_Next(receivedSecond);
/*
	printf("Old List Before Cut:\n");
	ListItr_ForEach(receivedFirst, receivedLast, PrintInt, NULL);
*/
	newList = ListItr_Cut(receivedFirst, receivedThird);
	ASSERT_THAT(newList != NULL);
	ASSERT_THAT(ListItr_Begin(newList) != NULL);
/*
	printf("Old List After Cut:\n");
	receivedFirst = ListItr_Begin(ls);
	ListItr_ForEach(receivedFirst, receivedLast, PrintInt, NULL);
*/
	printf("New List After Cut:\n");

	ListItr_ForEach(ListItr_Begin(newList), ListItr_End(newList), PrintInt, NULL);

	ListDestroy(&ls, NULL);
	ListDestroy(&newList, NULL);

END_UNIT
/*-----------------------Main ------------------------*/
TEST_SUITE(Generic_Double_Linked_List_Tests)

	TEST(List_Create_Normal)

	TEST(List_Destroy_Null)
	TEST(List_Destroy_DoubleFree)

	TEST(List_Push_head_Null)
	TEST(List_Push_head_One)
	TEST(List_Push_head_Many)
	TEST(List_Push_head_One_Then_Pop)
	TEST(List_Push_head_Many_Then_Pop)

	TEST(List_Pop_head_Null)
	TEST(List_Pop_head_From_Empty)
	TEST(List_Pop_Too_Many_Heads)

	TEST(Itr_Begin_null)
	TEST(Itr_Begin_OK)

	TEST(Itr_End_null)
	TEST(Itr_End_OK)

	TEST(Itr_Equal_Yes)
	TEST(Itr_Equal_No)

	TEST(Itr_Next_head)
	TEST(Itr_Next_tail)

	TEST(Itr_Prev_head)
	TEST(Itr_Prev_tail)

	TEST(Itr_Set_normal)

	TEST(Itr_InsertBefore_tail)
	TEST(Itr_InsertBefore_Normal)

	TEST(Itr_Remove_Null)
	TEST(Itr_Remove_normal)

	TEST(Func_Itr_For_Each_Normal)
	TEST(Func_Itr_For_Each_Null)

	TEST(Func_Itr_FindFirst_Null)
	TEST(Func_Itr_FindFirst_Normal)

	TEST(Func_Itr_CountIf_Null)
	TEST(Func_Itr_CountIf_Normal)

	TEST(Func_Itr_Splice_Normal)

	TEST(Func_Itr_Unique_Normal)

	TEST(Func_Itr_Cut_Normal)

END_SUITE


/*------------Itr Mini-Functions Declerations---------*/
int PrintInt(void* _element, void* _context){

	if (_element == NULL){
		return -1;
	}

	printf("The element is: %d\n", *(int*)ListItr_Get(_element));
	return 1;
}

int FindBiggerThanContext(void* _element, void* _context){

	return *(int*)ListItr_Get(_element) > *(int*)_context;
}

int AreIntsEqual(void* _elementA, void* _elementB){

	if(*(int*)ListItr_Get(_elementA) == *(int*)ListItr_Get(_elementB)){
		return EQUAL;
	}

	else{
		return NOT_EQUAL;
	}
}

