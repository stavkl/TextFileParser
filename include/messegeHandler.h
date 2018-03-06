#ifndef __MESSAGE_HANDLER_H__
#define __MESSAGE_HANDLER_H__
/** 
 *  @file 		messegeHandler.h
 *  @brief 		Provides basic functionalities for inserting and exporting data from a DS
 *  @details 	Functionalities include creating and updating entries based on a CDR 
 *				and printing a specific entry to a seperate file
 *  @author 	Stav Klein  (klein.stav@gmail.com)
 */
/**/
#include "messenger.h"
#include <stdio.h>

#define IMSI_SIZE 20
#define MSISDN_SIZE 20
#define MCC_MNC_SIZE 20
#define BRAND_NAME_SIZE 70

typedef struct Subscriber{
	char	m_msisdn[MSISDN_SIZE];
	char	m_imsi[IMSI_SIZE];
	/*Calls*/
	int		m_outCallsInOp;
	int		m_outCallsOutOp;
	int		m_inCallsInOp;
	int		m_inCallsOutOp;
	/*SMS*/
	int		m_nSMSSentInOp;
	int		m_nSMSSentOutOp;
	int		m_nSMSReceivedInOp;
	int		m_nSMSReceivedOutOp;
	/*Data*/
	double	m_mbDownloaded;
	double	m_mbUploaded;
}Subscriber;

typedef struct Operator{
	char	m_mccMnc[MCC_MNC_SIZE];
	char	m_brandName[BRAND_NAME_SIZE];
	/*Calls*/
	int		m_inCallsDur;
	int		m_outCallsDur;
	/*SMS*/
	int		m_inSMS;
	int		m_outSMS;
}Operator;

/** 
 * @brief 		Creates a subscriber struct from a given CDR message
 * @details 	CDR struct is specified in messenger.h file and is transferred here via MQ. It's information is extracted and
 *				put into a Subscribe struct, specified here.
 * @param[in] 	_cdrMsg: a pointer to the received CDR message
 * @return 		A pointer to the allocated subscriber struct
 */
Subscriber* CreateSubscriber(UniformCDR* _cdrMsg);

/** 
 * @brief 		Creates an operator struct from a given CDR message
 * @details 	CDR struct is specified in messenger.h file and is transferred here via MQ. It's information is extracted and
 *				put into an Operator struct, specified here.
 * @param[in] 	_cdrMsg: a pointer to the received CDR message
 * @return 		A pointer to the allocated operator struct
 */
Operator* CreateOperator(UniformCDR* _cdrMsg);

/** TODO: move to a seperate module once UI is complete
 * @brief 		Prints the given subscriber's data to the given file.		
 * @details 	Gets the accumulated data of the subscriber (at a given moment) and prints it to a file. Once the writing is
 *				completed, the file is closed and the subscriber's data is reset. 
 *				The file will be placed in a distinguished directory (should be specified in a config file)
 * @param[in] 	_subscriber: a pointer to the subscriber struct
 * @param[in] 	_outputFile: a pointer to a file (if a file with the same name already exists, it will be overridden)
 * @return 		void (prints to a file)				
 */
void PrintSubToFile(Subscriber* _subscriber, FILE* _outputFile);

/** TODO: move to a seperate module once UI is complete
 * @brief 		Prints the given operator's data to the given file.		
 * @details 	Gets the accumulated data of the operator (at a given moment) and prints it to a file. Once the writing is
 *				completed, the file is closed and the operator's data is reset. 
 *				The file will be placed in a distinguished directory (should be specified in a config file)
 * @param[in] 	_operator: a pointer to the subscriber struct
 * @param[in] 	_outputFile: a pointer to a file (if a file with the same name already exists, it will be overridden)
 * @return 		void (prints to a file)							
 */
void PrintOpToFile(Operator* _operator, FILE* _outputFile);

/** 
 * @brief 		Updates a given subscriber's data according to another subscriber's struct
 * @details 	Adds each field in the original subscriber (toUpdate), with its corresponding field in the new struct.
 *				This function is sent as a parameter to the Upsert function.
 * @param[in] 	_newValue: a pointer to a newly created struct, returned by CreateSubscriber
 * @param[in] 	_toUpdateValue: a pointer to the existing struct to be updated. In case no such struct exists,
 *				the _newValue will be inserted as a new entry.
 * @return 		void			
 */
void UpdateSub(void* _newValue, void* _toUpdateValue);

/** 
 * @brief 		Updates a given operator's data according to another operator's struct
 * @details 	Adds each field in the original operator (toUpdate), with its corresponding field in the new struct.
 *				This function is sent as a parameter to the Upsert function.
 * @param[in] 	_newValue: a pointer to a newly created struct, returned by CreateOperator
 * @param[in] 	_toUpdateValue: a pointer to the existing struct to be updated. In case no such struct exists,
 *				the _newValue will be inserted as a new entry.
 * @return 		void			
 */
void UpdateOp(void* _newValue, void* _toUpdateValue);

#endif /*__MESSAGE_HANDLER_H__*/
