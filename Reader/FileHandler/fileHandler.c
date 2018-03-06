#include "messenger.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#define DEF_FLAGS (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

/*------------------------------------------------------------------------------*/
/*					Static Functions Declerations (and details)					*/
/*------------------------------------------------------------------------------*/
/** 
 * @brief 		Gets the number of lines in a file (given that this number is specified in the file's header)
 * @param[in] 	_file: file pointer. the file must already be open for reading
 * @return 		The number specified in the header (should be the number of lines in the file)		
 */
int GetNumOfLines(FILE* _file);

/** 
 * @brief 		Gets the type of delimiter from a config file (given that this character is specified in the file's header)
 * @param[in] 	_file: file pointer. the file must already be open for reading
 * @return 		The char specified in the header (should be the character according to which the line-members are seperated)		
 */
char GetDelimiter(FILE* _file);

/** 
 * @brief 		Extracts a line from a file
 * @param[in] 	_file: file pointer. the file must already be open for reading
 * @param[in] 	_line: a buffer to read the line to
 * @return 		a pointer to the extracted line (will be returned to the given buffer)		
 */
char* GetLine(FILE* _file, char* _line);

/** 
 * @brief 		Creates a message struct and returns its pointer
 * @param[in] 	_msgType: specified channel for the message
 * @param[in] 	_msgContent: actual content of the message should not exceed 200 bytes
 * @param[in] 	_serial: the number of this message (out of all planned messages)
 * @return 		a pointer to the message struct			
 */
void MakeLineUniformedCDR(UniformCDR* _cdrMsg, char* _line, char* _delimiter);

/** 
 * @brief 		Creates a message struct and returns its pointer
 * @param[in] 	_msgType: specified channel for the message
 * @param[in] 	_msgContent: actual content of the message should not exceed 200 bytes
 * @param[in] 	_serial: the number of this message (out of all planned messages)
 * @return 		a pointer to the message struct			
 */
void MoveToDone(char* _oldName, char* _newName);



/*------------------------------------------------------------------------------*/
/*									Main										*/
/*------------------------------------------------------------------------------*/

/*
argv[0] - program name
argv[1] - char to send to ftok
argv[2] - name of config file (including path)
argv[3] - name of file (including path) to be opened (this is the path of a file already in the "Processing" folder)
argv[4] - name of file (including path) to be sent to when done

open config file (get delimiter)
open MQ
open CDR file (get lines one by one)
loop until EOF:

	parsing each line with strtok
	creating a CDR struct from that line
	calculating the size of that struct
	sending the struct to the MQ

upon finishing(aka after the specified n.o lines or EOF):
	move file to Done library
*/

int main(int argc,char* argv[]){

	FILE* newRecord;
	FILE* config;
	int numOfLines, i, qid;
	size_t msgSize;
	char line[CDR_LINE_SIZE];
	char delimiter;

	Msg msg;

	/*open config and get delimiter*/
	if(!(config = fopen(argv[2], "r"))){
		printf("Couldn't open file2\n");
		return 1;
	}
	delimiter = GetDelimiter(config);
	fclose(config);

	/*open message queue*/
	qid = MQ_CreateOrConnect();

	/*open CDR file, parse and send each line*/
	if(!(newRecord = fopen(argv[3], "r"))){
		printf("argv[3]: %s\n", argv[3]);
		printf("Couldn't open file3\n");
		return 1;
	}
	numOfLines = GetNumOfLines(newRecord);
	GetLine(newRecord, line); /*This is done so that fgets will go down one line*/


	msg.m_type = READER_SENDING_CHANNEL;
	printf("filename is: %s\n", argv[3]);
	for(i = 0; i < numOfLines; ++i){
		GetLine(newRecord, line);
		MakeLineUniformedCDR(&msg.m_payload.m_cdrMsg, line, &delimiter);

		msgSize = sizeof(msg.m_payload.m_cdrMsg);

		printf("Msg number %d, MsgSize: %lu\n", i, msgSize);
		printf("QID: %d\n", qid);

		MQ_MessageSend(qid, &msg, msgSize);
	}

	/*move file to 'Done' folder*/
	MoveToDone(argv[3], argv[4]);
	fclose(newRecord);

	return 0;
}



/*------------------------------------------------------------------------------*/
/*								GetNumOfLines									*/
/*------------------------------------------------------------------------------*/

int GetNumOfLines(FILE* _file){
	int number;
	fscanf(_file, "%d", &number);
	return number;
}

/*------------------------------------------------------------------------------*/
/*								GetDelimiter									*/
/*------------------------------------------------------------------------------*/
char GetDelimiter(FILE* _file){
	char del;
	fscanf(_file, "%c", &del);
	return del;
}

/*------------------------------------------------------------------------------*/
/*									GetLine										*/
/*------------------------------------------------------------------------------*/
char* GetLine(FILE* _file, char* _line){
	
	if(!fgets (_line, CDR_LINE_SIZE, _file)){
		printf("line is: %s\n", _line);	
		printf("Error getting line\n");
	}
	return _line;
}

/*------------------------------------------------------------------------------*/
/*							MakeLineUniformedCDR								*/
/*------------------------------------------------------------------------------*/
void MakeLineUniformedCDR(UniformCDR* _cdrMsg, char* _line, char* _delimiter){
	
	strcpy(_cdrMsg->m_imsi, strtok(_line, _delimiter));
	strcpy(_cdrMsg->m_msisdn, strtok(NULL, _delimiter));
	strcpy(_cdrMsg->m_subImei, strtok(NULL, _delimiter));
	strcpy(_cdrMsg->m_opBrandName, strtok(NULL, _delimiter));
	strcpy(_cdrMsg->m_opMccMnc, strtok(NULL, _delimiter));
	strcpy(_cdrMsg->m_callType, strtok(NULL, _delimiter));
	strcpy(_cdrMsg->m_callDate, strtok(NULL, _delimiter));
	strcpy(_cdrMsg->m_callTime, strtok(NULL, _delimiter));
	strcpy(_cdrMsg->m_duration, strtok(NULL, _delimiter));
	strcpy(_cdrMsg->m_mbDownloaded, strtok(NULL, _delimiter));
	strcpy(_cdrMsg->m_mbUploaded, strtok(NULL, _delimiter));
	strcpy(_cdrMsg->m_partyMsisdn, strtok(NULL, _delimiter));
	strcpy(_cdrMsg->m_partyOp, strtok(NULL, _delimiter));
}

/*------------------------------------------------------------------------------*/
/*								MoveToDone										*/
/*------------------------------------------------------------------------------*/
void MoveToDone(char* _oldName, char* _newName){
	rename(_oldName, _newName);
}


