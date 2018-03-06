#ifndef __MESSENGER_H__
#define __MESSENGER_H__

/** 
 *  @file messenger.h
 *  @brief 		Creates and provides basic functions for a message queue
 *  @details 	Functionalities include creating and destroying the MQ, creating, sending and receiving messages
 *  @author 	Stav Klein  (klein.stav@gmail.com)
 */
/**/
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stddef.h>

#define IMSI_SIZE 20
#define MSISDN_SIZE 20
#define SUBIMEI_SIZE 15
#define BRAND_NAME_SIZE 70
#define MCC_MNC_SIZE 20
#define CALL_TYPE_SIZE 10
#define DATE_SIZE 15
#define TIME_SIZE 15
#define DURATION_SIZE 10
#define DOWN_SIZE 10
#define UP_SIZE 10
#define PART_MSISDN_SIZE 20
#define PARTY_OP_SIZE 70
#define CDR_LINE_SIZE	(IMSI_SIZE + MSISDN_SIZE + SUBIMEI_SIZE + BRAND_NAME_SIZE +\
						MCC_MNC_SIZE + CALL_TYPE_SIZE + DATE_SIZE + TIME_SIZE + \
						DURATION_SIZE + DOWN_SIZE + UP_SIZE + PART_MSISDN_SIZE + \
						PARTY_OP_SIZE)

#define READER_SENDING_CHANNEL 1
#define CDR_CHANNEL 2
#define CTRL_CHANNEL 2

typedef enum UIMsg{
	PAUSE = 1,
	RESUME,
	SHUT_DOWN,
	REPORT_SUBSCRIBER,
	REPORT_OPERATOR
}UIMsg;

typedef struct 	CtrlMsg{
	int		m_instruction;
	char*	m_key;
}CtrlMsg;

typedef struct UniformCDR{
	char	m_imsi[IMSI_SIZE];
	char	m_msisdn[MSISDN_SIZE];
	char	m_subImei[SUBIMEI_SIZE];
	char	m_opBrandName[BRAND_NAME_SIZE];
	char	m_opMccMnc[MCC_MNC_SIZE];
	char	m_callType[CALL_TYPE_SIZE];
	char	m_callDate[DATE_SIZE];
	char	m_callTime[TIME_SIZE];
	char	m_duration[DURATION_SIZE];
	char	m_mbDownloaded[DOWN_SIZE];
	char	m_mbUploaded[UP_SIZE];
	char	m_partyMsisdn[PART_MSISDN_SIZE];
	char	m_partyOp[PARTY_OP_SIZE];
}UniformCDR;


typedef union Payload{
	CtrlMsg		m_ctrlMsg;
	UniformCDR	m_cdrMsg;
}Payload;


typedef struct Msg{
	long 	m_type;
	Payload	m_payload;		
}Msg;



/** 
 * @brief 		Creates or connects to a message queue 
 * @details 	calculates and returns the queue idetifier. To connect to the same queue two programs must give the same id
 * @param[in] 	_id: an identifying char (should also be the same for each process that wants to connect)
 * @return 		If successful, the return value will be the message queue identifier (a nonnegative integer), 
 *				otherwise -1 with errno indicating the error		
 */
int MQ_CreateOrConnect();/*TODO Done*/


/** 
 * @brief 		Creates a message struct and returns its pointer
 * @param[in] 	_msgType: specified channel for the message
 * @param[in] 	_msgContent: actual content of the message should not exceed 200 bytes
 * @param[in] 	_serial: the number of this message (out of all planned messages)
 * @return 		a pointer to the message struct			
 */
int MQ_MessageSend(int _queueID, Msg* _msg, size_t _msgSize);/*TODO Done*/

/** 
 * @brief 		Receives a message from the queue
 * @param[in] 	_queueID: The ID of the queue to send to (should have been received from MQ_InitializeQueue)  
 * @param[in] 	_msg: a pointer to the message created by MQ_MessageCreate
 * @param[in] 	_msgChannel: the channel upon which the message is sent
 * @return 		returns the number of bytes actually copied				
 */
ssize_t MQ_MessageReceive(int _queueID, Msg* _msg, long _msgChannel, int flags);

/** 
 * @brief 		Deletes a given queue according to its ID
 * @param[in] 	_queueID: The ID of the queue to send to (should have been received from MQ_InitializeQueue)  
 * @return 		0 on success		
 */
int MQ_RemoveQueue(int _queueID);


Msg* MQ_MessageCreate(long _msgType, const char* _msgContent, int _serialNum);
#endif /* __MESSENGER_H__ */
