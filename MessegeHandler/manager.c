#include "manager.h"
#include "messegeHandler.h"
#include "HashMap.h"
#include "messenger.h"
#include <stdlib.h> /*malloc*/
#include <time.h> /*localtime*/
#include <stdio.h>/*FILE*/
#include <string.h>/*strcmp*/


#define OPERATORS_NUM 200
#define MAGIC 0xD00FD00F
#define BLACK_MAGIC 0xFFFFFFFF
#define IS_A_CONTAINER(C)(C && C->m_magic == MAGIC)
#define OP_PATH "Billings/OP/"
#define SUB_PATH "Billings/SUB/"
#define NAME_SIZE 50

struct Container{
	size_t	 m_magic;
	HashMap* m_subscribers;
	HashMap* m_operators;

};
typedef struct Subscriber Subscriber;
typedef struct Operator Operator;
/************************************************************/
/*				static function decleration					*/
/************************************************************/
static int AreKeysEqual(const void* _firstKey, const void* _secondKey);
static size_t hashFunc(const void* _str);
void KeyDestroyer(void* _key);
void ValDestroyer(void* _value);
static void ResetSubscriber(void* _subscriber);
static void ResetOperator(void* _operator);
/************************************************************/
/*					Create Container						*/
/************************************************************/
Container* Mgr_CreateContainer(size_t _capacity){
	
	HashMap* subscribers = NULL;
	HashMap* operators = NULL;
	Container* container = NULL;
	
	subscribers = HashMap_Create(_capacity, hashFunc, AreKeysEqual);
	if(!subscribers){
		/*TODO log the error*/
		return NULL;
	}

	operators = HashMap_Create(OPERATORS_NUM, hashFunc, AreKeysEqual);
	if(!operators){
		free(subscribers);
		/*TODO log the error*/
		return NULL;
	}

	container = malloc(sizeof(Container));
	if(!container){
		free(subscribers);
		free(operators);
		/*TODO log the error*/
		return NULL;
	}

	container->m_subscribers = subscribers;
	container->m_operators = operators;
	container->m_magic = MAGIC;
	return container;
}

/************************************************************/
/*				static functions for Create					*/
/************************************************************/
static int AreKeysEqual(const void* _firstKey, const void* _secondKey){
	char* first = (char*)_firstKey;
	char* second = (char*)_secondKey;

	if(strcmp(first, second) == 0){
		return 1;
	}
	return 0;
}

static size_t hashFunc(const void* _str){
	unsigned char* s = (unsigned char*)_str;
    size_t hash = 5381;
    size_t c;

    while ((c = *(s++))){
        hash = ((hash << 5) + hash) + c;  /* hash * 33 + c */
	}

    return hash;
}

/************************************************************/
/*					Destroy Container						*/
/************************************************************/
void Mgr_DestroyContainer(Container** _container){

	if (!_container || !IS_A_CONTAINER((*_container))){
		return;
	}

	HashMap_Destroy(&(*_container)->m_subscribers, KeyDestroyer, ValDestroyer);
	HashMap_Destroy(&(*_container)->m_operators, KeyDestroyer, ValDestroyer);

	(*_container)->m_magic = BLACK_MAGIC;
	free(*_container);
	*_container = NULL;

}
/************************************************************/
/*				static functions for Destroy				*/
/************************************************************/
void KeyDestroyer(void* _key){
	_key = NULL;
}

void ValDestroyer(void* _value){
	free(_value);
}

/************************************************************/
/*					Get One Subscriber						*/
/************************************************************/
int Mgr_GetOneSubscriber(Container* _container, char* _msisdn){

	FILE* output;
	void* retrievedData;
	void* receivedKey;
	char filename[NAME_SIZE];
	char path[NAME_SIZE];

	if (!IS_A_CONTAINER(_container)){
		return 0;
	}

	/*Generate proper file name and path name*/
	strcpy(filename, "sub_");
	strcat(filename, _msisdn);
	strcat(filename, "_output.txt");

	strcpy(path, SUB_PATH);
	strcat(path, filename);
	
	if( HashMap_Remove(_container->m_subscribers, _msisdn, &receivedKey, &retrievedData) != MAP_SUCCESS){
		/*TODO log the error*/
		return 0;
	}
	
	/*printf("%s\n", path);*/
	if(!(output = fopen(filename, "w"))){
		printf("Couldn't open file\n"); /*TODO move to logger*/
	}

	PrintSubToFile(retrievedData, output);
	rename(filename, path);
	fclose(output);
	ResetSubscriber(retrievedData);
	return 1;
}

/************************************************************/
/*					Get One Operator						*/
/************************************************************/
int Mgr_GetOneOperator(Container* _container, char* _mccMnc){

	FILE* output;
	void* retrievedData;
	void* receivedKey;
	char filename[NAME_SIZE];
	char path[NAME_SIZE];

	if (!IS_A_CONTAINER(_container)){
		return 0;
	}
	/*Generate proper file name and path name*//*TODO consider seperate function*/
	strcpy(filename, "op_");
	strcat(filename, _mccMnc);
	strcat(filename, "_output.txt");
	strcpy(path, OP_PATH);
	strcat(path, filename);

	if(HashMap_Remove(_container->m_operators, _mccMnc, &receivedKey, &retrievedData) != MAP_SUCCESS){
		/*TODO log the error*/
		return 0;
	}

	if(!(output = fopen(filename, "w"))){
		printf("Couldn't open file\n"); /*TODO move to logger*/
	}

	PrintOpToFile(retrievedData, output);
	rename(filename, path);
	fclose(output);
	ResetOperator(retrievedData); 
	return 1;
}


/************************************************************/
/*					Upsert To Container 					*/
/************************************************************/
int Mgr_UpsertSub(Container* _container, void* _structS, UpdateValue _updateFunc){

	int error;

	error = HashMap_Upsert(_container->m_subscribers, ((Subscriber*)_structS)->m_msisdn, (Subscriber*)_structS, _updateFunc);
	if(error == MAP_VALUE_UPDATED){
		free(_structS);
		return error;
	}
	
	else if(error != MAP_SUCCESS){
		return error;
	}
	
	return MAP_SUCCESS;

}

int Mgr_UpsertOp(Container* _container, void* _structO, UpdateValue _updateFunc){

	int error;
	
	void* key = ((Operator*)_structO)->m_mccMnc;
	error = HashMap_Upsert(_container->m_operators, key, (Operator*)_structO, _updateFunc);
	if(error == MAP_VALUE_UPDATED){
		free(_structO);
	}
	
	else if(error != MAP_SUCCESS){
		return error;
	}

	return MAP_SUCCESS;

}

static void ResetSubscriber(void* _subscriber){
	Subscriber* sub = _subscriber;
	sub->m_outCallsInOp = 0;
	sub->m_outCallsOutOp = 0;
	sub->m_inCallsInOp = 0;
	sub->m_inCallsOutOp = 0;

	sub->m_nSMSSentInOp = 0;
	sub->m_nSMSSentOutOp = 0;
	sub->m_nSMSReceivedInOp = 0;
	sub->m_nSMSReceivedOutOp = 0;

	sub->m_mbDownloaded = 0;
	sub->m_mbUploaded = 0;
}

static void ResetOperator(void* _operator){
	Operator* op = _operator;
	op->m_inCallsDur = 0;
	op->m_outCallsDur = 0;

	op->m_inSMS = 0;
	op->m_outSMS = 0;
}



