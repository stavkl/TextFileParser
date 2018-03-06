#include "list.h"
#include "list_itr.h"
#include "list_functions.h"
#include "HashMap.h"
#include "mu_test.h"
#include <stdio.h>
#include <stdlib.h>
#define SIZE 10

/*-----------Helper Functions Declerations------------*/
size_t Modulu(const void* _data);
static int IsIntEqual(const void* _firstKey, const void* _secondKey);
void IntKeyDestroyer(void* _key);
void IntValDestroyer(void* _value);
void ValAddToSelf(void* _elementA, void* _elementB);
/*-------------------Create Tests --------------------*/
UNIT(HashMap_Create_Normal)
	HashMap* hashmap = NULL;
	hashmap = HashMap_Create(SIZE, Modulu, IsIntEqual);
	ASSERT_THAT(hashmap != NULL);
	HashMap_Destroy(&hashmap, IntKeyDestroyer, IntValDestroyer);

END_UNIT

UNIT(HashMap_Create_Null)
	HashMap* hashmap = NULL;
	hashmap = HashMap_Create(SIZE, NULL, NULL);
	ASSERT_THAT(hashmap == NULL);
END_UNIT

/*-------------------Insert Tests --------------------*/
UNIT(HashMap_Insert_Key_Null)
	int item0 = 99;
	HashMap* hashmap = NULL;
	hashmap = HashMap_Create(SIZE, Modulu, IsIntEqual);

	ASSERT_THAT(HashMap_Insert(hashmap, NULL, &item0) == MAP_KEY_NULL_ERROR);
	HashMap_Destroy(&hashmap, IntKeyDestroyer, IntValDestroyer);
	
END_UNIT

UNIT(HashMap_Insert_Normal)
	HashMap* hashmap = NULL;

	int* value = malloc(sizeof(int));
	int* value2 = malloc(sizeof(int));
	int* value3 = malloc(sizeof(int));
	*value = 1;
	*value2 = 1;
	*value3 = 2;

	hashmap = HashMap_Create(SIZE, Modulu, IsIntEqual);

	ASSERT_THAT(HashMap_Insert(hashmap, value, value) == MAP_SUCCESS);
	ASSERT_THAT(HashMap_Insert(hashmap, value2, value2) == MAP_KEY_DUPLICATE_ERROR);
	ASSERT_THAT(HashMap_Insert(hashmap, value3, value3) == MAP_SUCCESS);
	HashMap_Destroy(&hashmap, IntKeyDestroyer, IntValDestroyer);
	
END_UNIT

UNIT(HashMap_Insert_Duplicate)
	HashMap* hashmap = NULL;
	int* value1 = malloc(sizeof(int));
	*value1 = 1;
	hashmap = HashMap_Create(SIZE, Modulu, IsIntEqual);

	ASSERT_THAT(HashMap_Insert(hashmap, value1, value1) == MAP_SUCCESS);
	ASSERT_THAT(HashMap_Insert(hashmap, value1, value1) == MAP_KEY_DUPLICATE_ERROR);
	HashMap_Destroy(&hashmap, IntKeyDestroyer, IntValDestroyer);
	/*free(value1);*/
END_UNIT

/*-------------------Upsert Tests --------------------*/
UNIT(HashMap_Upsert_Insert)
	HashMap* hashmap = NULL;
	void* receivedVal1;
 	size_t searchKey1 = 1;
	int* value = malloc(sizeof(int));
	int* value3 = malloc(sizeof(int));
	*value = 1;
	*value3 = 2;

	hashmap = HashMap_Create(SIZE, Modulu, IsIntEqual);

	ASSERT_THAT(HashMap_Upsert(hashmap, value, value, NULL) == MAP_SUCCESS);
	ASSERT_THAT(HashMap_Upsert(hashmap, value3, value3, NULL) == MAP_SUCCESS);

	HashMap_Find(hashmap, &searchKey1, &receivedVal1);
	ASSERT_THAT(*value == *(int*)receivedVal1);

	HashMap_Destroy(&hashmap, IntKeyDestroyer, IntValDestroyer);
	
END_UNIT

UNIT(HashMap_Upsert_Update)
	HashMap* hashmap = NULL;
	void* receivedVal2 = NULL;
	size_t searchKey1 = 5;
	int* value = malloc(sizeof(int));
	int* value2 = malloc(sizeof(int));
	*value = 1;
	*value2 = 5;

	hashmap = HashMap_Create(SIZE, Modulu, IsIntEqual);

	ASSERT_THAT(HashMap_Upsert(hashmap, value2, value, NULL) == MAP_SUCCESS);
	ASSERT_THAT(HashMap_Upsert(hashmap, value, value2, NULL) == MAP_SUCCESS);

	ASSERT_THAT(HashMap_Upsert(hashmap, value2, value, ValAddToSelf) == MAP_VALUE_UPDATED);

	printf("Find res: %d\n", HashMap_Find(hashmap, &searchKey1, &receivedVal2));
	
	printf("Value2: %d\n", *(int*)receivedVal2);

	HashMap_Destroy(&hashmap, IntKeyDestroyer, IntValDestroyer);
	
END_UNIT
/*-------------------Remove Tests --------------------*/
UNIT(HashMap_Remove_HashMap_Null)
	ASSERT_THAT(HashMap_Remove(NULL, NULL, NULL, NULL) == MAP_UNINITIALIZED_ERROR);
END_UNIT

UNIT(HashMap_Remove_Key_Null)
	HashMap* hashmap = NULL;
	hashmap = HashMap_Create(SIZE, Modulu, IsIntEqual);
	ASSERT_THAT(HashMap_Remove(hashmap, NULL, NULL, NULL) == MAP_KEY_NULL_ERROR);
	HashMap_Destroy(&hashmap, IntKeyDestroyer, IntValDestroyer);
	
END_UNIT

UNIT(HashMap_Remove_From_Empty)
	HashMap* hashmap = NULL;
	int searchKey = 5;
	hashmap = HashMap_Create(SIZE, Modulu, IsIntEqual);
	ASSERT_THAT(HashMap_Remove(hashmap, &searchKey, NULL, NULL) == MAP_KEY_NOT_FOUND_ERROR);
	HashMap_Destroy(&hashmap, IntKeyDestroyer, IntValDestroyer);
	
END_UNIT

UNIT(HashMap_Remove_Normal)
	HashMap* hashmap = NULL;
	void* receivedVal6 = NULL;
	void* receivedKey6 = NULL;	
 	size_t searchKey1 = 6;
/*
	int value6 = 6;
	int value7 = 7;
	int value8 = 8;
*/

	int* value6 = (int*)malloc(sizeof(int));
	int* value7 = (int*)malloc(sizeof(int));
	int* value8 = (int*)malloc(sizeof(int));
	*value6 = 6;
	*value7 = 7;
	*value8 = 8;

	hashmap = HashMap_Create(SIZE, Modulu, IsIntEqual);

	HashMap_Insert(hashmap, value6, value6);
	HashMap_Insert(hashmap, value7, value7);
	HashMap_Insert(hashmap, value8, value8);

	printf("remove result: %d\n", HashMap_Remove(hashmap, &searchKey1, &receivedKey6, &receivedVal6));
	printf("key is %d, val is %d\n", *(int*)receivedKey6, *(int*)receivedVal6);

	/*ASSERT_THAT(HashMap_Remove(hashmap, &searchKey1, &receivedKey6, &receivedVal6) == MAP_SUCCESS);*/
	/*ASSERT_THAT(*value6 == *(int*)receivedKey6);*/
	/*ASSERT_THAT(*value6 == *(int*)receivedVal6);*/
	/*ASSERT_THAT(receivedKey6 == NULL);*/
	HashMap_Destroy(&hashmap, IntKeyDestroyer, IntValDestroyer);
	ASSERT_THAT(hashmap == NULL);
/*
	free(value6);
	free(value7);
	free(value8);
*/
END_UNIT

UNIT(HashMap_Find_Normal)
	HashMap* hashmap = NULL;
	void* receivedVal9;
 	size_t searchKey1 = 9;

	int* value9 = (int*)malloc(sizeof(int));
	int* value10 = (int*)malloc(sizeof(int));
	int* value11 = (int*)malloc(sizeof(int));
	*value9 = 9;
	*value10 = 10;
	*value11 = 11;

	hashmap = HashMap_Create(SIZE, Modulu, IsIntEqual);

	HashMap_Insert(hashmap, value9, value9);
	HashMap_Insert(hashmap, value10, value10);
	HashMap_Insert(hashmap, value11, value11);


	ASSERT_THAT(HashMap_Find(hashmap, &searchKey1, &receivedVal9) == MAP_SUCCESS);
	ASSERT_THAT(*value9 == *(int*)receivedVal9);
	HashMap_Destroy(&hashmap, IntKeyDestroyer, IntValDestroyer);
	ASSERT_THAT(hashmap == NULL);

END_UNIT


UNIT(HashMap_Size_Normal)
	HashMap* hashmap = NULL;
	
	int* value12 = (int*)malloc(sizeof(int));
	int* value13 = (int*)malloc(sizeof(int));
	int* value23 = (int*)malloc(sizeof(int));
	int* value14 = (int*)malloc(sizeof(int));
	*value12 = 12;
	*value13 = 13;
	*value23 = 23;
	*value14 = 14;

	hashmap = HashMap_Create(SIZE, Modulu, IsIntEqual);

	HashMap_Insert(hashmap, value12, value12);
	HashMap_Insert(hashmap, value13, value13);
	HashMap_Insert(hashmap, value23, value23);
	HashMap_Insert(hashmap, value14, value14);

	printf("size is %lu\n", HashMap_Size(hashmap));
	ASSERT_THAT(HashMap_Size(hashmap) == 4);
	HashMap_Destroy(&hashmap, IntKeyDestroyer, IntValDestroyer);
	ASSERT_THAT(hashmap == NULL);

END_UNIT

UNIT(HashMap_Rehash_Up)
	HashMap* hashmap = NULL;
	
	int* value12 = (int*)malloc(sizeof(int));
	int* value13 = (int*)malloc(sizeof(int));
	int* value23 = (int*)malloc(sizeof(int));
	int* value14 = (int*)malloc(sizeof(int));
	*value12 = 12;
	*value13 = 13;
	*value23 = 23;
	*value14 = 14;

	hashmap = HashMap_Create(SIZE, Modulu, IsIntEqual);

	HashMap_Insert(hashmap, value12, value12);
	HashMap_Insert(hashmap, value13, value13);
	HashMap_Insert(hashmap, value23, value23);
	HashMap_Insert(hashmap, value14, value14);

	HashMap_Rehash(hashmap, SIZE+(SIZE/2));
	
	HashMap_Destroy(&hashmap, IntKeyDestroyer, IntValDestroyer);
	ASSERT_THAT(hashmap != NULL);

END_UNIT

UNIT(HashMap_Rehash_Down)
	HashMap* hashmap = NULL;
	
	int* value12 = (int*)malloc(sizeof(int));
	int* value13 = (int*)malloc(sizeof(int));
	int* value23 = (int*)malloc(sizeof(int));
	int* value14 = (int*)malloc(sizeof(int));
	*value12 = 12;
	*value13 = 13;
	*value23 = 23;
	*value14 = 14;

	hashmap = HashMap_Create(SIZE, Modulu, IsIntEqual);

	HashMap_Insert(hashmap, value12, value12);
	HashMap_Insert(hashmap, value13, value13);
	HashMap_Insert(hashmap, value23, value23);
	HashMap_Insert(hashmap, value14, value14);

	HashMap_Rehash(hashmap, SIZE/2);
	
	HashMap_Destroy(&hashmap, IntKeyDestroyer, IntValDestroyer);
	ASSERT_THAT(hashmap != NULL);

END_UNIT



/*-----------------------Main ------------------------*/
TEST_SUITE(Generic_Hash_Map)
/*
	TEST(HashMap_Create_Normal)
	TEST(HashMap_Create_Null)

	TEST(HashMap_Insert_Key_Null)
	TEST(HashMap_Insert_Normal)
	TEST(HashMap_Insert_Duplicate)
*/
	TEST(HashMap_Upsert_Insert)
	TEST(HashMap_Upsert_Update)

/*
	TEST(HashMap_Remove_HashMap_Null)
	TEST(HashMap_Remove_Key_Null)
	TEST(HashMap_Remove_From_Empty)
	TEST(HashMap_Remove_Normal)

	TEST(HashMap_Find_Normal)

	TEST(HashMap_Size_Normal)

	TEST(HashMap_Rehash_Up)
	TEST(HashMap_Rehash_Down)
*/
END_SUITE


/*------------ Test-Functions Declerations------------*/

/*NOT A GOOD Hash Function*/
size_t Modulu(const void* _data){
	return (size_t)(*(int*)_data);
}

/*Equality Function*/
static int IsIntEqual(const void* _firstKey, const void* _secondKey){
	return *(int*)_firstKey == *(int*)_secondKey;
}

void IntKeyDestroyer(void* _key){
	free(_key);
}

void IntValDestroyer(void* _value){
	_value = NULL;
}

void ValAddToSelf(void* _elementA, void* _elementB){
	*(int*)_elementB += *(int*)_elementA;
}

