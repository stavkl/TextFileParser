#include "messenger.h"
#include "HashMap.h"
#include "messegeHandler.h"
#include "manager.h"
#include <stdio.h>
#include <stdlib.h> /*malloc*/
#include <time.h> /*localtime*/
#include <stdio.h>/*FILE*/
#include <string.h>/*strcmp*/
#include <pthread.h>

#define IN_MINUTES(N)((N)/60.0)
#define	IN_HOURS(N)((N)/3600.0)
#define CAPACITY 1000000
#define NUM_OF_THREADS 4

struct Container{
	size_t	 m_magic;
	HashMap* m_subscribers;
	HashMap* m_operators;
};

typedef struct ThreadPackage{
	int			m_msqid;
	Container* 	m_container;
}ThreadPackage;


typedef struct ProcessorPkg{
	pthread_t* 		m_threads;
	ThreadPackage*	m_package;
}ProcessorPkg;

/************************************************************/
/*				Static functions declerations				*/
/************************************************************/
static Subscriber* SetSubscriber(Subscriber* _subscriber, UniformCDR* _cdrMsg);
static Operator* SetOperator(Operator* _operator, UniformCDR* _cdrMsg);
void* ThreadFunc(void* _package);
void* CtrlThreadFunc(void* _package);
void DestroyMsgHandler(ProcessorPkg* _pkg);
ProcessorPkg* CreateMsgHandler();
/************************************************************/
/*							Main							*/
/************************************************************/
int main(){

	ProcessorPkg* pkg;

	pkg = CreateMsgHandler();

	
	DestroyMsgHandler(pkg);

	return 0;
}


/************************************************************/
/*					Create Subscriber						*/
/************************************************************/
Subscriber* CreateSubscriber(UniformCDR* _cdrMsg){
	
	Subscriber* newSubscriber;
		
	newSubscriber = calloc(1, sizeof(Subscriber));
	if(!newSubscriber){	
		/*TODO log error*/
		return NULL;
	}
	
	newSubscriber = SetSubscriber(newSubscriber, _cdrMsg);
	
	return newSubscriber;
}


/************************************************************/
/*						Set Subscriber						*/
/************************************************************/
static Subscriber* SetSubscriber(Subscriber* _subscriber, UniformCDR* _cdrMsg){

	strcpy(_subscriber->m_msisdn, _cdrMsg->m_msisdn);
	strcpy(_subscriber->m_imsi, _cdrMsg->m_imsi);

	/*Update Subscriber's Calls*/
	if(strcmp(_cdrMsg->m_callType, "MOC")){/*outgoing voice call*/
		if(strcmp(_cdrMsg->m_partyOp, _cdrMsg->m_opBrandName)){/*within operator*/
			_subscriber->m_outCallsInOp += atoi(_cdrMsg->m_duration);
		}
		
		else if(!strcmp(_cdrMsg->m_partyOp, _cdrMsg->m_opBrandName)){/*outside operator*/
			_subscriber->m_outCallsOutOp += atoi(_cdrMsg->m_duration);
		}

	}
	if(strcmp(_cdrMsg->m_callType, "MTC")){/*incoming voice call*/
		if(strcmp(_cdrMsg->m_partyOp, _cdrMsg->m_opBrandName)){/*within operator*/
			_subscriber->m_inCallsInOp += atoi(_cdrMsg->m_duration);
		}
		
		else if(!strcmp(_cdrMsg->m_partyOp, _cdrMsg->m_opBrandName)){/*outside operator*/
			_subscriber->m_inCallsOutOp += atoi(_cdrMsg->m_duration);
		}

	}

	/*Update Subscriber's SMS*/
	if(strcmp(_cdrMsg->m_callType, "SMS-MO")){/*outgoing message*/
		if(strcmp(_cdrMsg->m_partyOp, _cdrMsg->m_opBrandName)){/*within operator*/
			++_subscriber->m_nSMSSentInOp;
		}
		
		else if(!strcmp(_cdrMsg->m_partyOp, _cdrMsg->m_opBrandName)){/*outside operator*/
			++_subscriber->m_nSMSSentOutOp;
		}

	}
	if(strcmp(_cdrMsg->m_callType, "SMS-MT")){/*incoming message*/
		if(strcmp(_cdrMsg->m_partyOp, _cdrMsg->m_opBrandName)){/*within operator*/
			++_subscriber->m_nSMSReceivedInOp;
		}
		
		else if(!strcmp(_cdrMsg->m_partyOp, _cdrMsg->m_opBrandName)){/*outside operator*/
			++_subscriber->m_nSMSReceivedOutOp;
		}

	}

	/*Update Subscriber's Data*/
	if(strcmp(_cdrMsg->m_callType, "GPRS")){/*internet*/
		_subscriber->m_mbDownloaded += (double)atof(_cdrMsg->m_mbDownloaded);
		_subscriber->m_mbUploaded += (double)atof(_cdrMsg->m_mbUploaded);
	}

	return _subscriber;
}

/************************************************************/
/*					Create Operator							*/
/************************************************************/
Operator* CreateOperator(UniformCDR* _cdrMsg){
	
	Operator* newOperator;
		
	newOperator = calloc(1, sizeof(Operator));
	if(!newOperator){	
		/*TODO log error*/
		return NULL;
	}
	
	newOperator = SetOperator(newOperator, _cdrMsg);
	return newOperator;
}

/************************************************************/
/*					Update Operator							*/
/************************************************************/
static Operator* SetOperator(Operator* _operator, UniformCDR* _cdrMsg){

	strcpy(_operator->m_mccMnc, _cdrMsg->m_opMccMnc);
	strcpy(_operator->m_brandName, _cdrMsg->m_opBrandName);

	/*Update Operator's Calls*/
	if(strcmp(_cdrMsg->m_callType, "MOC")){/*outgoing voice call*/
		
		_operator->m_outCallsDur += atoi(_cdrMsg->m_duration);
	}

	if(strcmp(_cdrMsg->m_callType, "MTC")){/*incoming voice call*/
		
		_operator->m_outCallsDur += atoi(_cdrMsg->m_duration);
	}

	/*Update Operator's SMS*/
	if(strcmp(_cdrMsg->m_callType, "SMS-MO")){/*outgoing message*/

		++_operator->m_outSMS;
	}

	if(strcmp(_cdrMsg->m_callType, "SMS-MT")){/*incoming message*/

		++_operator->m_inSMS;
	}

	return _operator;

}

/************************************************************/
/*				Print subscriber to file					*/
/************************************************************/
void PrintSubToFile(Subscriber* _subscriber, FILE* _outputFile){
	time_t rawtime;
	struct tm * timeinfo;

	time (&rawtime);
	timeinfo = localtime(&rawtime);

	fprintf(_outputFile, 
"%s\nIMSI: %s, Outgoing calls within operator (in minutes): %f,\
 Outgoing calls outside operator (in minutes): %f, Incoming calls within operator (in minutes): %f,\
 Incoming calls outside operator (in minutes): %f, Outgoing SMS within operator: %f,\
 Outgoing SMS outside operator: %f, Incoming SMS within operator: %f,\
 Incoming SMS outside operator: %f, MB downloaded: %f, MB uploaded: %f\n",
asctime (timeinfo), _subscriber->m_imsi, (double)IN_MINUTES(_subscriber->m_outCallsInOp),
(double)IN_MINUTES(_subscriber->m_outCallsOutOp), (double)IN_MINUTES(_subscriber->m_inCallsInOp), (double)IN_MINUTES(_subscriber->m_inCallsOutOp),
(double)_subscriber->m_nSMSSentInOp, (double)_subscriber->m_nSMSSentOutOp,
(double)_subscriber->m_nSMSReceivedInOp, (double)_subscriber->m_nSMSReceivedOutOp,
_subscriber->m_mbDownloaded, _subscriber->m_mbUploaded);
}


/************************************************************/
/*				Print Operator to file						*/
/************************************************************/
void PrintOpToFile(Operator* _operator, FILE* _outputFile){
	time_t rawtime;
	struct tm * timeinfo;

	time ( &rawtime );
	timeinfo = localtime(&rawtime);

	fprintf(_outputFile, 
"%s\nMCC/MNC: %s, Brand Name: %s,\
 Incoming calls duration (in hours): %f,Outgoing calls duration (in hours): %f,\
 Incoming SMS messages: %f,Outgoing SMS messages: %f\n",
asctime (timeinfo), _operator->m_mccMnc, _operator->m_brandName,
IN_HOURS(_operator->m_inCallsDur), IN_HOURS(_operator->m_outCallsDur),
(double)_operator->m_inSMS, (double)_operator->m_outSMS);
}

/************************************************************/
/*				Update Subscriber from DB					*/
/************************************************************/
void UpdateSub(void* _newValue, void* _toUpdateValue){

	Subscriber* originalSub = (Subscriber*)_toUpdateValue;
	Subscriber* updates = (Subscriber*)_newValue;

	originalSub->m_outCallsInOp += updates->m_outCallsInOp;
	originalSub->m_outCallsOutOp += updates->m_outCallsOutOp;
	originalSub->m_inCallsInOp += updates->m_inCallsInOp;
	originalSub->m_inCallsOutOp += updates->m_inCallsOutOp;

	originalSub->m_nSMSSentInOp += updates->m_nSMSSentInOp;
	originalSub->m_nSMSSentOutOp += updates->m_nSMSSentOutOp;
	originalSub->m_nSMSReceivedInOp += updates->m_nSMSReceivedInOp;
	originalSub->m_nSMSReceivedOutOp += updates->m_nSMSReceivedOutOp;

	originalSub->m_mbDownloaded += updates->m_mbDownloaded;
	originalSub->m_mbUploaded += updates->m_mbUploaded;
	
}

/************************************************************/
/*				Update Operator from DB						*/
/************************************************************/
void UpdateOp(void* _newValue, void* _toUpdateValue){

	Operator* originalOp = (Operator*)_toUpdateValue;
	Operator* updates = (Operator*)_newValue;

	originalOp->m_inCallsDur += updates->m_inCallsDur;
	originalOp->m_outCallsDur += updates->m_outCallsDur;
	originalOp->m_inSMS += updates->m_inSMS;
	originalOp->m_outSMS += updates->m_outSMS;
}

/************************************************************/
/*					Thread Func								*/
/************************************************************/
void* ThreadFunc(void* _package){

	Subscriber* sub = NULL;
	Operator* op = NULL;
	ThreadPackage* pkg = _package;
	Msg receivedCDR;

	while(1){

		printf("QID: %d\n", pkg->m_msqid);
		MQ_MessageReceive(pkg->m_msqid, &receivedCDR, READER_SENDING_CHANNEL, 0);

		sub = CreateSubscriber(&receivedCDR.m_payload.m_cdrMsg);
		op = CreateOperator(&receivedCDR.m_payload.m_cdrMsg);

		Mgr_UpsertSub(pkg->m_container, sub, UpdateSub);
		Mgr_UpsertOp(pkg->m_container, op, UpdateOp);
	}
}

/************************************************************/
/*						CtrlThreadFunc						*/
/************************************************************/
void* CtrlThreadFunc(void* _package){
	Msg receivedCtrl;
	ThreadPackage* pkg = _package;

	while(1){

		if(MQ_MessageReceive(pkg->m_msqid, &receivedCtrl, CTRL_CHANNEL, IPC_NOWAIT)){
			if(receivedCtrl.m_payload.m_ctrlMsg.m_instruction == SHUT_DOWN){
				/*wait for all threads to finish and produce report*/
			}
			else if(receivedCtrl.m_payload.m_ctrlMsg.m_instruction == PAUSE){
				/*send msg to reader to pause*/
			}
			else if(receivedCtrl.m_payload.m_ctrlMsg.m_instruction == REPORT_SUBSCRIBER){
				Mgr_GetOneSubscriber(pkg->m_container, receivedCtrl.m_payload.m_ctrlMsg.m_key);
			}
			else if(receivedCtrl.m_payload.m_ctrlMsg.m_instruction == REPORT_OPERATOR){
				Mgr_GetOneOperator(pkg->m_container, receivedCtrl.m_payload.m_ctrlMsg.m_key);
			}
		}
	}
}


/************************************************************/
/*					CreateMsgHandler						*/
/************************************************************/
ProcessorPkg* CreateMsgHandler(){
	
	ThreadPackage* threadPackage = NULL;
	ProcessorPkg* procPkg = NULL;
	Container* container = NULL;
	pthread_t* threads = NULL;
	int msqid, i;
	
	/*creates thread package*/
	container = Mgr_CreateContainer(CAPACITY);
	if(!container){
		perror("Create Msg Handler - Container create");
		exit(1);
	}
	msqid = MQ_CreateOrConnect();
	threadPackage = malloc(sizeof(ThreadPackage));
	if(!threadPackage){
		perror("Create Msg Handler - Thread Package malloc");
		Mgr_DestroyContainer(&container);
		exit(1);	
	}
	threadPackage->m_msqid = msqid;
	threadPackage->m_container = container;
	

	/*creates processor package*/
	threads = malloc(sizeof(pthread_t) * NUM_OF_THREADS);
	if(!threads){
		perror("Create Msg Handler - Processor Package Thread malloc");
		Mgr_DestroyContainer(&container);
		free(threadPackage);
		exit(1);
	}
	for (i = 0; i < NUM_OF_THREADS - 1; ++i){
		pthread_create(&threads[i], NULL, ThreadFunc, threadPackage);
	}
	pthread_create(&threads[i], NULL, CtrlThreadFunc, threadPackage);

	procPkg = malloc(sizeof(ProcessorPkg));
	if(!procPkg){
		perror("Create Msg Handler - Processor Package procPkg malloc");
		Mgr_DestroyContainer(&container);
		free(threadPackage);
		free(threads);
		exit(1);		
	}
	procPkg->m_threads = threads;
	procPkg->m_package = threadPackage;

	return procPkg;
}

/************************************************************/
/*					Destroy MsgHandler						*/
/************************************************************/
void DestroyMsgHandler(ProcessorPkg* _pkg){
	int i;
	for (i = 0; i < NUM_OF_THREADS; ++i){
		pthread_join(_pkg->m_threads[i], NULL);
	}
	free(_pkg->m_threads);
	Mgr_DestroyContainer(&(_pkg)->m_package->m_container);
	free(_pkg->m_package);
	free(_pkg);
}



