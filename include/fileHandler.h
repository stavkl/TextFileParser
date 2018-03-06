#ifndef __FILE_HANDLER__
#define __FILE_HANDLER__
#include "messenger.h"
#include "parser.h"
#include <stdio.h>
/** 
 *  @file 		fileHandler.h
 *  @brief 		Creates and provides basic functions for handling a single file
 *  @details 	Functionalities include parsing a line from a file, moving the file to a directory
 *  @author 	Stav Klein  (klein.stav@gmail.com)
 *
/**/

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
UniformCDR* MakeLineUniformedCDR(UniformCDR* _cdrMsg, char* _line, char* _delimiter);

/** 
 * @brief 		Creates a message struct and returns its pointer
 * @param[in] 	_msgType: specified channel for the message
 * @param[in] 	_msgContent: actual content of the message should not exceed 200 bytes
 * @param[in] 	_serial: the number of this message (out of all planned messages)
 * @return 		a pointer to the message struct			
 */
int SendOverMQ(int _queueID, Msg* _msg, int _msgSize);

/** 
 * @brief 		Creates a message struct and returns its pointer
 * @param[in] 	_msgType: specified channel for the message
 * @param[in] 	_msgContent: actual content of the message should not exceed 200 bytes
 * @param[in] 	_serial: the number of this message (out of all planned messages)
 * @return 		a pointer to the message struct			
 */
void MoveToDone(char* _oldName, char* _newName);



#endif /*__FILE_HANDLER__*/
