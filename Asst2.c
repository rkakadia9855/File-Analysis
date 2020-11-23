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

struct fileinfo {
    char* name;
    pthread_t* thread;
};

struct wordnode {
    char* name;
    int totalCount;
    struct wordnode *nextWord;
};

struct filenode{
    char* name;
    struct filenode *nextFile; 
    struct wordnode *headWord;
};

void *grow(struct fileinfo smt[100], int* capacityPTR) {
    //update capacity
    printf("growing\n");
    return NULL;
} 

void* handleFile(void* kmt) {
    int numLoop = 0;
    struct fileinfo * funcArgs = (struct fileinfo *) kmt;
    char* name = funcArgs->name;

    printf("Handling: %s\n", name);
    //pthread_t thread = args->thread;
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
	struct fileinfo * funcArgs = (struct fileinfo  *) kmt;
    int numLoop = 0;
    char* name = funcArgs->name;

    printf("Handling: %s\n", name);
    //pthread_t thread = args->thread;
    
    if(strcmp(name, "smt"))
        numLoop = 10000000;
    else if(strcmp(name, "dmt"))
        numLoop = 10000;
    for(int i = 0 ; i < numLoop; i++) {
	    int k = 0;
    }
    printf("handled: %s\n", name);
}

int main(int argc, char** argv) {

    if(argc != 2) {
        printf("Usage: %s file\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    struct fileinfo arr[100];
    pthread_t allThreads[100];

    int capacity = 100;
    int* capacityPTR = &capacity;


    int tracker = 0;

    DIR *dirp = opendir(argv[1]);
    struct dirent *dp;
    while((dp = readdir(dirp))) {
        if(tracker >= capacity) {
            grow(arr, capacityPTR);
        }
    	if(dp->d_type == DT_REG) {
            printf("Got a file: %s\n", dp->d_name);
            
            struct fileinfo temp;
            temp.name = dp->d_name;
            temp.thread  = allThreads+tracker;
            arr[tracker] = temp;
           
            pthread_create(allThreads+tracker, NULL, handleFile, &temp);

            printf("thread number %ld added to arr\n", *(allThreads+tracker));
            tracker++;
        }
        else if(dp->d_type == DT_DIR) {
            printf("Got a dir: %s\n", dp->d_name);
            
            struct fileinfo temp;
            temp.name = dp->d_name;
            temp.thread  = allThreads+tracker;
            arr[tracker] = temp;

             pthread_create(allThreads+tracker, NULL, handleDir, &temp);

            printf("thread number %ld added to arr\n", *(allThreads+tracker));
            tracker++;
        }
    //    tracker++;
    }
    int i = 0;
    for (i = 0; i < tracker; ++i) {
        printf("Joining: %ld\n", allThreads[i]);
        pthread_join(allThreads[i], NULL);
    }
    closedir(dirp);
    
    return EXIT_SUCCESS;
}
