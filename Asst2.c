#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <ctype.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pthread.h>

pthread_t allThreads[100]; //stores the handles of all threads
int tracker = -1; //tracks the total number of files/directories and threads at the same time
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

struct wordnode {
    char* name;
    int totalCount;
    struct wordnode *nextWord;
};

struct filenode{
    char* name;
    struct filenode *nextFile; 
    struct wordnode *headWord;
  //  pthread_t thread;
    int index;
 //   int* trackerPtr;
};

struct filenode files[100]; //stores the nodes of files/directories

void *grow(struct filenode smt[100], int* capacityPTR) {
    //update capacity
    printf("growing\n");
    return NULL;
} 

void* handleFile(void* kmt) {
    int numLoop = 0;
    struct filenode * funcArgs = (struct filenode *) kmt;
    char* name = funcArgs->name;

    printf("Handling: %s\n", name);
    if(strcmp(name, "testcases.txt"))
        numLoop = 100000000;
    else if(strcmp(name, "readme.pdf"))
        numLoop = 10000000;
    else if(strcmp(name, "tokenizer.c"))
        numLoop = 1000000;
    for(int i = 0 ; i < numLoop; i++) {
	    int k = 0;
    }
    printf("handled: %s\n", name);
}

void* handleDir(void* kmt) {
	struct filenode * funcArgs = (struct filenode  *) kmt;
    int numLoop = 0;
    char* name = funcArgs->name;

    printf("Handling: %s\n", name);
    printf("%s was added at %d\n", name, funcArgs->index);

    DIR *dirp = opendir(name);
    if(dirp != NULL) {
        struct dirent *dp;
        while((dp = readdir(dirp))) { 
            if(dp->d_type == DT_REG) {
                pthread_mutex_lock(&lock);
                tracker++;
                int totalLen = strlen(name) + 2 + strlen(dp->d_name);
                char* tempName = (char *) malloc(sizeof(char *) * totalLen);
                strcpy(tempName, name);
                strcat(tempName, "/");
                strcat(tempName, dp->d_name);
                files[tracker].name = (char *) malloc(sizeof(char *) * totalLen);
                strcpy(files[tracker].name, tempName);
                files[tracker].index = tracker;
            //   files[tracker].trackerPtr = &tracker;
                pthread_create(allThreads+tracker, NULL, handleFile, &(files[tracker]));
                printf("thread number %ld added to arr\n", *(allThreads+tracker));
                pthread_mutex_unlock(&lock); 
            }
            else if(dp->d_type == DT_DIR && strcmp(".", dp->d_name) != 0 && strcmp("..", dp->d_name) != 0) {
                pthread_mutex_lock(&lock);
                tracker++;
                int totalLen = strlen(name) + 2 + strlen(dp->d_name);
                char* tempName = (char *) malloc(sizeof(char *) * totalLen);
                strcpy(tempName, name);
                strcat(tempName, "/");
                strcat(tempName, dp->d_name);
                files[tracker].name = (char *) malloc(sizeof(char *) * totalLen);
                strcpy(files[tracker].name, tempName);
                files[tracker].index = tracker;
            //   files[tracker].trackerPtr = &tracker;
                pthread_create(allThreads+tracker, NULL, handleDir, &(files[tracker]));
                printf("thread number %ld added to arr\n", *(allThreads+tracker));
                pthread_mutex_unlock(&lock); 
            }
        }
    }
    printf("handled: %s\n", name);
    closedir(dirp);
}

int main(int argc, char** argv) {

    if(argc != 2) {
        printf("Usage: %s file\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    

    int capacity = 100;
    int* capacityPTR = &capacity;

    DIR *dirp = opendir(argv[1]);
    struct dirent *dp;
    while((dp = readdir(dirp))) {
        /*if(tracker >= capacity) {
            grow(arr, capacityPTR);
        } */
        
    	if(dp->d_type == DT_REG) {
            pthread_mutex_lock(&lock);    
            tracker++;
            printf("Got a file: %s\n", dp->d_name);
            
            int totalLen = 9;
            totalLen += strlen(dp->d_name);
            char* tempName = (char *) malloc(sizeof(char *) * totalLen);
            tempName[0] = '.';
            tempName[1] = '/';
            tempName[2] = 'A';
            tempName[3] = 's';
            tempName[4] = 's';
            tempName[5] = 't';
            tempName[6] = '0';
            tempName[7] = '/';
            strcat(tempName, dp->d_name);
            files[tracker].name = (char *) malloc(sizeof(char *) * totalLen);
            strcpy(files[tracker].name, tempName);
            files[tracker].index = tracker;
         //   files[tracker].trackerPtr = &tracker;
            pthread_create(allThreads+tracker, NULL, handleFile, &(files[tracker]));
            printf("thread number %ld added to arr\n", *(allThreads+tracker));
            pthread_mutex_unlock(&lock);
        }
        else if(dp->d_type == DT_DIR && strcmp(".", dp->d_name) != 0 && strcmp("..", dp->d_name) != 0) {
            pthread_mutex_lock(&lock);    
            tracker++;
            printf("Got a dir: %s\n", dp->d_name);
            
            int totalLen = 9;
            totalLen += strlen(dp->d_name);
            char* tempName = (char *) malloc(sizeof(char *) * totalLen);
            tempName[0] = '.';
		    tempName[1] = '/';
            tempName[2] = 'A';
            tempName[3] = 's';
            tempName[4] = 's';
            tempName[5] = 't';
            tempName[6] = '0';
            tempName[7] = '/';
		    strcat(tempName, dp->d_name);
	    	files[tracker].name = (char *) malloc(sizeof(char *) * totalLen);
            strcpy(files[tracker].name, tempName);
            files[tracker].index = tracker;
            pthread_create(allThreads+tracker, NULL, handleDir, &(files[tracker]));
            printf("thread number %ld added to arr\n", *(allThreads+tracker));
            pthread_mutex_unlock(&lock);
        }
    }
    int i = 0;
    for (i = 0; i <= tracker; ++i) {
        printf("Joining: %ld\n", allThreads[i]);
        pthread_join(allThreads[i], NULL);
    }
    closedir(dirp);
    
    return EXIT_SUCCESS;
}
