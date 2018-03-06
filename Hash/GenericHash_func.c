#include "list.h"
#include "list_itr.h"
#include "list_functions.h"
#include "HashMap.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <pthread.h>
#define MAGIC 0xFEEDF00D
#define BLACK_MAGIC 0xDEADF00D
#define EMPTY 0
#define IS_A_MAP(M)(M && M->m_magic == MAGIC)
#define NUM_OF_MUTEXES(N)((N)/1000)
#define RELEVANT_MUTEX(N)((N) % 1000)

pthread_mutex_t* mutexes;

typedef struct DestroyPack DestroyPack;
struct DestroyPack{
	KeyDestroy			m_keyDestFunc;
	ValDestroy 			m_valDestFunc;
};

typedef struct KeyCompare KeyCompare;
struct KeyCompare{
	void* 				m_key;
	EqualityFunction 	m_keysEqualFunc;
};

typedef struct Pair Pair;
struct Pair{
	void* 	m_key;
	void* 	m_value;
};


struct HashMap{
	size_t 			m_magic;
	List* 			*m_buckets; /*array of pointers to list*/
	size_t 			m_size;
	HashFunction 	m_hashFunc;
	EqualityFunction m_keysEqualFunc;
	/*Map_Stats* 		m_mapStats;*/
};

/*------------------------------------Functions Declerations----------------------------------------*/
static ListItr IsKeyFound(const HashMap* _map, void* _key, size_t Bucket);
int CmpKeywFunc(ListItr _listItr, KeyCompare* _context);
int DestroyAccordingTo(ListItr _listItr, DestroyPack* _destroyer);
/*
static size_t IsPrime(size_t num);
static size_t NextPrime(size_t num);
*/
/*------------------------------------HashMap Create------------------------------------------------*/
/*
static size_t NextPrime(size_t num){
    size_t i = num + 1;
    while(1)
    {
        if(IsPrime(i))
            break;
        ++i;
    }
    return i;
}


static size_t IsPrime(size_t num){
	size_t i;
	double root;
	if(num % 2 == 0 || num % 3 == 0){
		return 0;
	}
    root = sqrt((double)num);

    for (i = 5; i <= root; i += 6){
        if (num % i == 0)
           return 0;
    }

    for (i = 7; i <= root; i += 6){
        if (num % i == 0)
           return 0;
    }

    return 1;
}
*/
HashMap* HashMap_Create(size_t _capacity, HashFunction _hashFunc, EqualityFunction _keysEqualFunc){
	int i;
	/*size_t primedSize;*/
	HashMap* hashMap = NULL;
	/*Map_Stats* tempStats;*/
	List** temp = NULL;
	
	if(_capacity == EMPTY || !_hashFunc || !_keysEqualFunc){
		return NULL;
	}	
	
	hashMap = (HashMap*)malloc(sizeof(HashMap));
	if (!hashMap){
		return NULL;
	}

	mutexes = malloc(sizeof(pthread_mutex_t) * NUM_OF_MUTEXES(_capacity) + 1);
	if (!mutexes){
		free(hashMap);
		return NULL;
	}
	for(i = 0; i <NUM_OF_MUTEXES(_capacity); ++i){
		pthread_mutex_init(&(mutexes[i]), NULL);
	}
	
	/*primedSize = NextPrime(_capacity);*/
	hashMap->m_buckets = NULL;
	hashMap->m_size = _capacity;
	hashMap->m_hashFunc = _hashFunc;
	hashMap->m_keysEqualFunc =_keysEqualFunc;
	hashMap->m_magic = MAGIC;

	temp = (List**)calloc(_capacity, sizeof(List*));
	if(!temp){
		free(mutexes);
		free(hashMap);
		return NULL;
	}
	else{
		hashMap->m_buckets = temp;
	}
/*TODO: put in NDEBUG*/
/*
	tempStats = (Map_Stats*)calloc(1, sizeof(Map_Stats));
	if(!tempStats){
		free(hashMap->m_buckets);
		free(hashMap);
		return NULL;
	}
	else{
		hashMap->m_mapStats = tempStats;
	}
*/
	return hashMap;
}

/*----------------------------------------Hash Upsert-----------------------------------------------*/

Map_Result HashMap_Upsert(HashMap* _map, void* _key, void* _value, UpdateValue _updateFunc){
	size_t bucketIndex;
	void* receivedData = NULL;
	int error;

	if(!IS_A_MAP(_map)){
		return MAP_UNINITIALIZED_ERROR;
	}
	if(!_key){
		return MAP_KEY_NULL_ERROR;
	}
	
	bucketIndex = _map->m_hashFunc(_key) % _map->m_size;
	/*printf("bucketIndex in upsert: %lu\n", bucketIndex);*/

	pthread_mutex_lock(&(mutexes[RELEVANT_MUTEX(bucketIndex)]));

	if((error = HashMap_Find(_map, _key, &receivedData)) == MAP_SUCCESS){
		_updateFunc(_value, receivedData);
		pthread_mutex_unlock(&(mutexes[RELEVANT_MUTEX(bucketIndex)]));
		return MAP_VALUE_UPDATED;	
	}

	else if(error != MAP_KEY_NOT_FOUND_ERROR){
		pthread_mutex_unlock(&(mutexes[RELEVANT_MUTEX(bucketIndex)]));
		return error;
	}

	HashMap_Insert(_map, _key, _value);
/*
	printf("Key from Upsert: %s\n", (char*)_key);
*/
	pthread_mutex_unlock(&(mutexes[RELEVANT_MUTEX(bucketIndex)]));
	return MAP_SUCCESS;
}
/*----------------------------------------Hash Insert-----------------------------------------------*/

Map_Result HashMap_Insert(HashMap* _map, void* _key, const void* _value){

	size_t bucketIndex;
	Pair* newPair = NULL;
	List* list = NULL;

	if(!IS_A_MAP(_map)){
		return MAP_UNINITIALIZED_ERROR;
	}
	if(!_key){
		return MAP_KEY_NULL_ERROR;
	}
	
	bucketIndex = _map->m_hashFunc(_key) % _map->m_size;
	pthread_mutex_lock(&(mutexes[RELEVANT_MUTEX(bucketIndex)]));
	if(IsKeyFound(_map, _key, bucketIndex)){
		pthread_mutex_unlock(&(mutexes[RELEVANT_MUTEX(bucketIndex)]));
		return MAP_KEY_DUPLICATE_ERROR;	
	}
	if (!_map->m_buckets[bucketIndex]){
		list = ListCreate();
		if (!list){
			pthread_mutex_unlock(&(mutexes[RELEVANT_MUTEX(bucketIndex)]));
			return MAP_ALLOCATION_ERROR;
		}
		_map->m_buckets[bucketIndex] = list;
	}


	if(!(newPair = (Pair*)malloc(sizeof(Pair)))){
		pthread_mutex_unlock(&(mutexes[RELEVANT_MUTEX(bucketIndex)]));
		return MAP_ALLOCATION_ERROR;
	}

	newPair->m_key = (void*)_key;
	newPair->m_value = (void*)_value;
	/*printf("key is %d, value is %d\n", *(int*)_key,*(int*)_value);*/
	ListPushHead(_map->m_buckets[bucketIndex], newPair);
	


	/*printf("begin key is %d\n", *(int*)((Pair*)ListItr_Get(ListItr_Begin(_map->m_buckets[bucketIndex])))->m_key);*/
	pthread_mutex_unlock(&(mutexes[RELEVANT_MUTEX(bucketIndex)]));
	return MAP_SUCCESS;
}
/*--------------------------------------Hash Is Key Found-------------------------------------------*/
static ListItr IsKeyFound(const HashMap* _map, void* _key, size_t Bucket){

	ListItr begin = NULL;	
	ListItr end = NULL;
	ListItr result = NULL;
	KeyCompare keyCmp;

	/*printf("is key found: %s\n", (char*)_key);*/	
	keyCmp.m_key = _key;
	keyCmp.m_keysEqualFunc = _map->m_keysEqualFunc;
	
	begin = ListItr_Begin(_map->m_buckets[Bucket]);
	end = ListItr_End(_map->m_buckets[Bucket]);

	result = ListItr_FindFirst(begin, end, (PredicateFunction)CmpKeywFunc, &keyCmp);
	return result == end ? NULL : result;
}

int CmpKeywFunc(ListItr _listItr, KeyCompare* _context){
	int result;
	result = _context->m_keysEqualFunc(((Pair*)ListItr_Get(_listItr))->m_key, _context->m_key);
	/*printf("key compare is: %d\n", *(int*)((Pair*)ListItr_Get(_listItr))->m_key);
	printf("result should be 1: %d\n", result);*/
	return result;  /*returns 1 if equal*/
}

/*------------------------------------------Hash Remove---------------------------------------------*/

Map_Result HashMap_Remove(HashMap* _map, void* _searchKey, void** _pKey, void** _pValue){

	size_t bucketIndex = 1;
	ListItr receivedItr = NULL;
	Pair* receivedPair = NULL;
	
	if(!IS_A_MAP(_map)){
		return MAP_UNINITIALIZED_ERROR;
	}
	if(!_searchKey){
		return MAP_KEY_NULL_ERROR;
	}
	
	bucketIndex = _map->m_hashFunc(_searchKey) % _map->m_size;

	pthread_mutex_lock(&(mutexes[RELEVANT_MUTEX(bucketIndex)]));

	if (!_map->m_buckets[bucketIndex]){
		pthread_mutex_unlock(&(mutexes[RELEVANT_MUTEX(bucketIndex)]));
		return MAP_KEY_NOT_FOUND_ERROR;
	}
	
	receivedItr = IsKeyFound(_map, _searchKey, bucketIndex);
	if (receivedItr){
		receivedPair = ListItr_Remove(receivedItr);
		*(_pKey) = receivedPair->m_key;
		*(_pValue) = receivedPair->m_value;
		pthread_mutex_unlock(&(mutexes[RELEVANT_MUTEX(bucketIndex)]));
		return MAP_SUCCESS;
		
	}
	pthread_mutex_unlock(&(mutexes[RELEVANT_MUTEX(bucketIndex)]));
	return MAP_KEY_NOT_FOUND_ERROR;
}

/*------------------------------------------Hash Destroy--------------------------------------------*/

void HashMap_Destroy(HashMap** _map, KeyDestroy _keyDestroyer, ValDestroy _valDestroyer){
	size_t numOfBuckets = 0;
	int i = 0;
	ListItr begin = NULL;
	ListItr end = NULL;
	DestroyPack multiDestroyer;

	/*check params - two pointers to map and two functions must not be null*/
	if (!_map || !IS_A_MAP((*_map))){
		return;
	}

	/*loop through buckest:
		get begin and end of each list		
		For-each item in between: destroy key, destroy value, free item->m_data(pair)
		free (empty) list
	*/
	multiDestroyer.m_keyDestFunc = _keyDestroyer;
	multiDestroyer.m_valDestFunc = _valDestroyer;

	numOfBuckets = (*_map)->m_size;
	for (i = 0; i < numOfBuckets; ++i){
		/*printf("got inside the loop %d times\n", i);*/
		begin = ListItr_Begin((*_map)->m_buckets[i]);
		end = ListItr_End((*_map)->m_buckets[i]);
		ListItr_ForEach(begin, end, (ListActionFunction)DestroyAccordingTo, &multiDestroyer);
		ListDestroy(&((*_map)->m_buckets[i]), NULL);
	}

	/*free array of buckets*/
	free((*_map)->m_buckets);

	/*free stats TODO: put in debug mode*/
		/*free((*_map)->m_mapStats)*/

	/*free Map*/
	(*_map)->m_magic = BLACK_MAGIC;
	free(*_map);
	*_map = NULL;

}

int DestroyAccordingTo(ListItr _listItr, DestroyPack* _destroyer){
	/*	both keyDestFunc and valDestFunc are defined as returning void,
		but ActionFunction for For-Each must return an int
	*/
	Pair* pair = NULL;

	if (_listItr == NULL || _destroyer == NULL){
		return 0;
	}
	pair = ListItr_Get(_listItr);
	
	if(_destroyer->m_keyDestFunc){
		_destroyer->m_keyDestFunc(pair->m_key);
	}

	if(_destroyer->m_valDestFunc){
		_destroyer->m_valDestFunc(pair->m_value);
	}
	
	free(pair);
	return 1;
}

/*------------------------------------------Hash Find Key-------------------------------------------*/
Map_Result HashMap_Find(const HashMap* _map, void* _searchKey, void** _pValue){

	Pair* pair = NULL;
	ListItr receivedItr = NULL;
	size_t bucketIndex = 0;

	/*printf("find key: %s\n", (char*)_searchKey);*/

	if (!_map){
		return MAP_UNINITIALIZED_ERROR;
	}
	if(!_searchKey){
		return MAP_KEY_NULL_ERROR;
	}

	bucketIndex = _map->m_hashFunc(_searchKey) % _map->m_size;
	
	receivedItr = IsKeyFound(_map, _searchKey, bucketIndex);

	if(receivedItr){
		pair = ListItr_Get(receivedItr);
		*_pValue = pair->m_value;
		pthread_mutex_unlock(&(mutexes[RELEVANT_MUTEX(bucketIndex)]));
		return MAP_SUCCESS;
	}

	return MAP_KEY_NOT_FOUND_ERROR;
	
}


/*------------------------------------------Hash Map Size-------------------------------------------*/

size_t HashMap_Size(const HashMap* _map){

	size_t numOfBuckets = 0;
	int i = 0;
	size_t pairCounter = 0;


	numOfBuckets = _map->m_size;

	for(i = 0; i < numOfBuckets; ++i){
	/*if list is null then ListSize is 0*/
		pairCounter += ListSize(_map->m_buckets[i]);
	}

	return pairCounter;
}

/*------------------------------------------Hash Rehash---------------------------------------------*/

Map_Result HashMap_Rehash(HashMap *_map, size_t _newCapacity){

	int i = 0;
	size_t numOfBuckets = 0;
	HashMap* newHashMap = NULL;
	Pair* pair = NULL;
	ListItr begin = NULL;
	ListItr end = NULL;
	ListItr temp = NULL;

	newHashMap = HashMap_Create(_newCapacity, _map->m_hashFunc, _map->m_keysEqualFunc);
	
	if (!newHashMap){
		return MAP_ALLOCATION_ERROR;
	}

	numOfBuckets = _map->m_size;
	for(i = 0; i < numOfBuckets; ++i){
		/*printf("got inside bucket %d\n", i);*/
		begin = ListItr_Begin(_map->m_buckets[i]);
		end = ListItr_End(_map->m_buckets[i]);

		/*getting the data of each node, reindex it and insert it to the new table*/
		while(begin != end){
			/*printf("Got INTO while-loop!\n");*/
			temp = ListItr_Next(begin);
			pair = ListItr_Remove(begin);
			HashMap_Insert(newHashMap, pair->m_key, pair->m_value); /*rehashing is done here*/
			begin = temp;
		}
		/*printf("Got to AFTER while-loop!\n");*/
	}
	HashMap_Destroy(&_map, NULL, NULL);
	_map = newHashMap;
	return MAP_SUCCESS;
}




