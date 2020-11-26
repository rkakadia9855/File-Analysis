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

#define BUFSIZE 256

pthread_t allThreads[100]; //stores the handles of all threads
int tracker = -1; //tracks the total number of files/directories and threads at the same time
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
int dirTracker = -1;
int threadTracker = -1;

struct wordnode {
    char* name;
    int totalCount;
    double probability;
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

struct dirnode {
    char* name;
};

struct filenode files[100]; //stores the nodes of files
struct dirnode directory[100]; //stores the nodes of directories

void printFileArr() {
	int i = 0; 
	for(i = 0; i <= tracker; i++) {
		if(i != tracker) {
			printf("File name: %s\n", files[i].name);
			printf("Next file: %s\n", files[i].nextFile->name);
		}
		else {
			printf("File name: %s\n", files[i].name);
		}
	}
}

void *grow(struct filenode smt[100], int* capacityPTR) {
    //update capacity
    printf("growing\n");
    return NULL;
} 

void* handleFile(void* kmt) {
    int numLoop = 0;
    struct filenode * funcArgs = (struct filenode *) kmt;
    char* name = funcArgs->name;

    printf("\n\n");
    printf("inspecting %s\n", name);

    int fd = open(name, O_RDONLY);
    if (fd < 0) {
        perror(name);
        exit(EXIT_FAILURE);
    }

    char buf[BUFSIZE];
    int bytes, pos;
    int filesize = 0;
    struct stat st;
    stat(name, &st);
    filesize = (int) st.st_size;

    char* buffer = (char *) malloc(sizeof(char) * filesize);
    bytes = read(fd, buffer, filesize);
    if(bytes < 0)
        printf("error\n");

    int j = 0;
    printf("num chars: %d\n", filesize);
    for(j = 0; j < strlen(buffer); j++) {
        buffer[j] = tolower(buffer[j]);
    }
    
    char* token = strtok(buffer, " \n\t\r\v\f");
    // could check here whether bytes is negative
    //  bytes == 0  - end of file
    //  bytes <  0  - error
printf("jsl\n");
    struct wordnode **head;
    head = &(funcArgs->headWord);
    printf("head assigned\n");
    struct wordnode *nodeTracker =  *(head);//struct wordnode*) malloc(sizeof(struct wordnode));
//    nodeTracker =  *(funcArgs->headWord);
    printf("added head\n");
    struct wordnode* new_node = (struct wordnode*) malloc(sizeof(struct wordnode));
    struct wordnode *prevNode;
    int infi = 0;

    while (token != NULL) { 
//	    printf("%s\n", token);
        if(nodeTracker == NULL) {
		    printf("head is null\n");
            new_node->name = token;
            new_node->totalCount = 1;
            new_node->probability = 1/filesize;
            new_node->nextWord = NULL;
            *(head) = new_node;
            prevNode = *(head);
            nodeTracker = *(head);
            printf("head word: %s\n", nodeTracker->name);
        }
        else {
//		printf("head is not null\n");
            while(nodeTracker != NULL) {
                infi++;
                printf("current word: %s\n", nodeTracker->name);
                printf("next word null? %d\n", (nodeTracker->nextWord == NULL));
//                printf("next word name %s\n", nodeTracker->nextWord->name);
                if(strcmp(nodeTracker->name, token) == 0) {
                    nodeTracker->totalCount = nodeTracker->totalCount + 1;
                    nodeTracker->probability = nodeTracker->totalCount/filesize;
                    printf("incremented count for : %s\n", nodeTracker->name);
                    break;
                }
              /*  if(nodeTracker->nextWord == NULL) {
                    if(strcmp(nodeTracker->name, token) == 0) {
                        nodeTracker->totalCount = nodeTracker->totalCount + 1;
                        nodeTracker->probability = nodeTracker->totalCount/filesize;
                        printf("incremented count for : %s\n", nodeTracker->name);
                        break;
                    }
                    else {
                        struct wordnode addWord;
                        addWord.name = token;
                        addWord.totalCount = 1;
                        addWord.probability = 1/filesize;
                        nodeTracker->nextWord = &addWord;
                        printf("added at the end : %s\n", nodeTracker->nextWord->name);
                        break;
                    }
                } */
//		printf("here\n");
                prevNode = nodeTracker;
//		printf("threre\n");
                nodeTracker = nodeTracker->nextWord;
//:wq
//printf("no where\n");
            }
        }
        if(nodeTracker == NULL) {
            new_node->name = token;
            new_node->totalCount = 1;
            new_node->probability = 1/filesize;
            new_node->nextWord = NULL;
            prevNode->nextWord = new_node;
	    printf("%s added\n", prevNode->nextWord->name);
	    printf("prev node: %s\n", prevNode->name);
        }
        token = strtok(NULL, " \n\t\r\v\f"); 
    }

    struct wordnode** tmp;
    tmp = &(funcArgs->headWord);
    struct wordnode* parseList;
    parseList = *(tmp);
    printf("head: %s\n", parseList->name);
    parseList = parseList->nextWord;
    printf("next: %s\n", parseList->name);
    parseList = parseList->nextWord;
    printf("next: %s\n", parseList->name);

/*	nodeTracker = funcArgs->headWord;
	while(nodeTracker != NULL) {
		printf("%s ---> ", nodeTracker->name);
		nodeTracker = nodeTracker->nextWord;
	}
	printf("NULL\n"); */

    close(fd);
    

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
                threadTracker++;
                tracker++;
                int totalLen = strlen(name) + 2 + strlen(dp->d_name);
                char* tempName = (char *) malloc(sizeof(char *) * totalLen);
                strcpy(tempName, name);
                strcat(tempName, "/");
                strcat(tempName, dp->d_name);
                files[tracker].name = (char *) malloc(sizeof(char *) * totalLen);
                strcpy(files[tracker].name, tempName);
                files[tracker].index = tracker;
		if(tracker != 0)
			files[tracker - 1].nextFile = &(files[tracker]);
            //   files[tracker].trackerPtr = &tracker;
                pthread_create(allThreads+threadTracker, NULL, handleFile, &(files[tracker]));
                printf("thread number %ld added to arr\n", *(allThreads+threadTracker));
                pthread_mutex_unlock(&lock); 
            }
            else if(dp->d_type == DT_DIR && strcmp(".", dp->d_name) != 0 && strcmp("..", dp->d_name) != 0) {
                pthread_mutex_lock(&lock);
                threadTracker++;
                dirTracker++;
                int totalLen = strlen(name) + 2 + strlen(dp->d_name);
                char* tempName = (char *) malloc(sizeof(char *) * totalLen);
                strcpy(tempName, name);
                strcat(tempName, "/");

                strcat(tempName, dp->d_name);
                directory[dirTracker].name = (char *) malloc(sizeof(char *) * totalLen);
                strcpy(directory[dirTracker].name, tempName);
                pthread_create(allThreads+threadTracker, NULL, handleDir, &(directory[dirTracker]));
               
                printf("thread number %ld added to arr\n", *(allThreads+threadTracker));
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
            threadTracker++;
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
	    if(tracker != 0)
		    files[tracker - 1].nextFile = &(files[tracker]);
         //   files[tracker].trackerPtr = &tracker;
            pthread_create(allThreads+threadTracker, NULL, handleFile, &(files[tracker]));
            printf("thread number %ld added to arr\n", *(allThreads+threadTracker));
            pthread_mutex_unlock(&lock);
        }
        else if(dp->d_type == DT_DIR && strcmp(".", dp->d_name) != 0 && strcmp("..", dp->d_name) != 0) {
            pthread_mutex_lock(&lock);    
            threadTracker++;
            dirTracker++;
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
            directory[dirTracker].name = (char *) malloc(sizeof(char *) * totalLen);
            strcpy(directory[dirTracker].name, tempName);
            pthread_create(allThreads+threadTracker, NULL, handleDir, &(directory[dirTracker]));
            printf("thread number %ld added to arr\n", *(allThreads+threadTracker));
            pthread_mutex_unlock(&lock);
        }
    }
    int i = 0;
    for (i = 0; i <= threadTracker; ++i) {
        printf("Joining: %ld\n", allThreads[i]);
        pthread_join(allThreads[i], NULL);
    }

    printf("Threads: %d\n", threadTracker);
    printf("Directories: %d\n", dirTracker);
    printf("Files: %d\n", tracker);
    
    printFileArr();
    closedir(dirp);
    
    return EXIT_SUCCESS;
}
