#ifndef __READER_H__
#define __READER_H__
#include "messenger.h"
#include <stdio.h>
/** 
 *  @file 		reader.h
 *  @brief 		Creates and provides basic functions for reading a directory
 *  @details 	Functionalities include checking the directory, move a file to another directory and controlling
 *				multi-processed file-handling
 *  @author 	Stav Klein  (klein.stav@gmail.com)
**/

void CheckDirectory (char* _dirPath, char* _receivedFile);
int IsAValidExtension(char* _fileName);
void MoveToDir(char* _fromPath, char* _toPath, char* _fileName);
char* GetNewPathName(char* _path, char* _fileName);



#endif /*__READER_H__*/
