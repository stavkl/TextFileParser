#include "messenger.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>

#define DEF_FLAGS (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#define CORRECT 0
#define DEF_PATH "/tmp/text.txt"


/*------------------------------------------------------------------------------*/
/*							Message Queue Initializer							*/
/*------------------------------------------------------------------------------*/
int MQ_CreateOrConnect(){

	/*key_t key;*/
	int qId;

	/*key = ftok(DEF_PATH, atoi("a"));*/

	if((qId = msgget(231088, DEF_FLAGS | IPC_CREAT)) == -1){
		perror("MQ_InitializeQueue");
		exit(EXIT_FAILURE);
	}

	return qId;
}


/*------------------------------------------------------------------------------*/
/*								Message Send									*/
/*------------------------------------------------------------------------------*/
int MQ_MessageSend(int _queueID, Msg* _msg, size_t _msgSize){

	if(!_msg){
		perror("Sending Null Message");
		exit(EXIT_FAILURE);
	}

	if(msgsnd(_queueID, _msg, _msgSize, 0) == -1){
		perror("MQ_MessageSend");
		exit(EXIT_FAILURE);		
	}
	return 0;
}

/*------------------------------------------------------------------------------*/
/*								Message Receive									*/
/*------------------------------------------------------------------------------*/
ssize_t MQ_MessageReceive(int _queueID, Msg* _msg, long _msgChannel, int flags){

	if(!_msg){
		perror("Receiving Null Message");
		exit(EXIT_FAILURE);
	}

	if(msgrcv(_queueID, _msg, sizeof(_msg->m_payload), _msgChannel, flags) == -1){
		perror("MQ_MessageReceive");
		exit(EXIT_FAILURE);
	}
	return 1;
}

/*------------------------------------------------------------------------------*/
/*								Remove Queue									*/
/*------------------------------------------------------------------------------*/
int MQ_RemoveQueue(int _queueID){

	return msgctl(_queueID, IPC_RMID, NULL);
}








