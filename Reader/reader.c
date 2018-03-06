#include "reader.h"
#include "messenger.h"
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>


#define INPUT_DIR "Input/"
#define PROCESSING_DIR "./FileHandler/Processing/"
#define DONE_DIR "./FileHandler/Done/"
#define ERR_PATH "Error/"
#define FTOK_CHAR "a"
#define EXTENSION_SIZE 5
#define NAME_SIZE 50

static void ReaderFlow();

int main(){

	ReaderFlow();

	return 0;
}


static void ReaderFlow(){
	char* argsForChild[] = {"./FileHandler/fileHandler", "a", "./FileHandler/config.txt", NULL, NULL, NULL};
	int status;

	while(1){
		char* finished = NULL;
		char receivedFile[40];
		char inputPath[NAME_SIZE] = INPUT_DIR;
		char processingPath[NAME_SIZE] = PROCESSING_DIR;
		char donePath[NAME_SIZE] = DONE_DIR;
		CheckDirectory(INPUT_DIR, receivedFile);
			
		MoveToDir(inputPath, processingPath, receivedFile);
		finished = 	GetNewPathName(donePath, receivedFile);
		printf("processing Path: %s\nfinished Path: %s\n", processingPath, finished);

		pid_t pid = fork();

		if(pid == 0 ){

			argsForChild[3] = processingPath;
			argsForChild[4] = finished;
			status = execvp(argsForChild[0] ,argsForChild);
			if(status == -1)
			{
				
				perror("Execve:");
			}
		}
		else if (pid > 0 ){
			wait(&pid);
		}

		else{
			printf("error in creating process\n");
		}
	}
}

void CheckDirectory (char* _dirPath, char* _receivedFile){
	DIR *dirp;
    struct dirent *dp;

	if(!(dirp = opendir(_dirPath))) {
        perror("Couldn't open directory");
        exit(1);
    }

	while (1) {
		dp = readdir(dirp);
		if(!dp){
			rewinddir(dirp);
		}
	
		else if(!strcmp(dp->d_name, ".") || !strcmp(dp->d_name, "..") ){
			 continue;
		} 

		else{
			strcpy(_receivedFile, dp->d_name);
			break;
		}
	}
	closedir(dirp);
	return;
}


int IsAValidExtension(char* _fileName){
	char txt[EXTENSION_SIZE]; /*for text file extensions + end of string char + 1*/
 	int i, j;

	if(strchr(_fileName,'.')){
		i = (int)strlen(_fileName);
		j = 0;
		txt[0]='\0';

		while(_fileName[i] !='.') {
			txt[j] = _fileName[i];
			++j;
			--i;
		}
	}

	if(strcmp(txt, "cdr") == 0){
		return 1;
	}
	
	return 0;
}

void MoveToDir(char* _fromPath, char* _toPath, char* _fileName){
	size_t fromP0;
	size_t toP0;

	strcat(_fromPath, _fileName);
	strcat(_toPath, _fileName);

	fromP0 = strlen(_fromPath) + strlen(_fileName);
	toP0 = strlen(_toPath) + strlen(_fileName);

	_fromPath[fromP0] = '\0';
	_toPath[toP0] = '\0';

	rename(_fromPath, _toPath);
	
}

char* GetNewPathName(char* _path, char* _fileName){
	size_t pathP0;

	strcat(_path, _fileName);
	pathP0 = strlen(_path) + strlen(_fileName);
	_path[pathP0] = '\0';

	return _path;
}



