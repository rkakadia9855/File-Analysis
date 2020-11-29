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
#include <errno.h>
#include <math.h>

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
    int wordcount;
  //  pthread_t thread;
    int index;
 //   int* trackerPtr;
};

struct dirnode {
    char* name;
};

struct filenode* headFile;
struct filenode* prevFile;

struct filenode files[100]; //stores the nodes of files
struct dirnode directory[100]; //stores the nodes of directories

/* function to swap data of two nodes a and b*/
void swap(struct filenode *a, struct filenode *b) 
{ 

    char* tempname = a->name;
    struct wordnode *tempheadWord = a->headWord;
    int tempwordcount = a->wordcount;

    a->name = b->name;
    a->headWord = b->headWord;
    a->wordcount = b->wordcount;

    b->name = tempname;
    b->headWord = tempheadWord;
    b->wordcount = tempwordcount;
    
} 

void sortWordsInFile() {
    int swapped, i; 
    struct filenode *ptr1; 
    struct filenode *lptr = NULL; 
    struct filenode *start = headFile;
  
    /* Checking for empty list */
    if (start == NULL) 
        return; 
  
    do
    { 
        swapped = 0; 
        ptr1 = start; 
  
        while (ptr1->nextFile != lptr) 
        { 
            if(ptr1->wordcount > ptr1->nextFile->wordcount)
            {  
                swap(ptr1, ptr1->nextFile); 
                swapped = 1; 
            } 
            ptr1 = ptr1->nextFile; 
        } 
        lptr = ptr1; 
    } 
    while (swapped); 
}

void printFileArr() {
	int i = 0; 
    struct filenode* fileTracker;
    fileTracker = headFile;
    while(fileTracker != NULL) {
        printf("%s ---> ", fileTracker->name);
        fileTracker = fileTracker->nextFile;
        if(fileTracker == NULL)
            break;
    }
    printf("NULL\n");
}

/*void findMeanDistribution() {
    struct filenode* fileTracker;
    struct wordnode* meanTracker;
    struct wordnode* wordTracker;
    fileTracker = headFile;
    while(fileTracker != NULL) {

       // for each file, compare every word in the file with every word in the mean distribution

       wordTracker = fileTracker->headWord;
       while(wordTracker != NULL) {

           if(meanhead == NULL) {
               meanhead = (struct wordnode*) malloc(sizeof(struct wordnode));
               strcpy(meanhead->name, wordTracker->name);
               meanhead->probability = wordTracker->probability;
               meanTracker->nextWord = NULL;
               meanTracker = meanhead;
           }
           else {
               while(meanTracker != NULL) {

                   if(strcmp(meanTracker->name, wordTracker->name) == 0) {
                       meanTracker->probability = meanTracker->probability + wordTracker->probability;
                       meanTracker->probability = meanTracker->probability/2;
                   }

                   meanTracker = meanTracker->nextWord;
               }
           }

           wordTracker = wordTracker->nextWord;
           if(wordTracker == NULL)
                break;
       }

        fileTracker = fileTracker->nextFile;
        if(fileTracker == NULL)
            break;
    }
} */

void outputJensenShannon() {

    // for each file, compare it with every file after it
    struct filenode* fileTracker = headFile;
    while(fileTracker != NULL) { // loop for each file
//	    printf("entered first loop\n");
  	printf("First file: %s\n", fileTracker->name);
	struct filenode* compareFile = fileTracker;
        struct filenode* secondaryFileTracker = compareFile->nextFile;
//	printf("secondary file assigned is first loop\n");
        //comparing the file with every other file that comes after it in linked list
        while(secondaryFileTracker != NULL) { // loop to compare with files after it
//		printf("entered second loop\n");
        
            // fill the linked list of mean distribution with first file
		printf("Second file: %s\n", secondaryFileTracker->name);
    	    struct wordnode* meanhead = (struct wordnode* ) malloc(sizeof(struct wordnode));
            struct wordnode* meanTracker = meanhead;
            struct wordnode* compareTracker = fileTracker->headWord;
            while(compareTracker != NULL) {
//		    printf(
		    printf("entered third loop\n");
		    meanTracker->name = (char *) malloc(sizeof(char*) * (int) strlen(compareTracker->name));
                strcpy(meanTracker->name, compareTracker->name);
		printf("Word added to mean distribution list: %s\n", meanTracker->name);
		printf("copied name in third loop\n");
                meanTracker->probability = compareTracker->probability;
                if(compareTracker->nextWord == NULL) {
//			printf
                    meanTracker->nextWord = NULL;
                    break;
                }
                meanTracker->nextWord = (struct wordnode* ) malloc(sizeof(struct wordnode));
		printf("next assigned in third loop\n");
                meanTracker = meanTracker->nextWord;
                compareTracker = compareTracker->nextWord;
		printf("loop next roung in third loop\n");
            }

            //Time to compare the two files now and update mean distribution linked list
            compareTracker = secondaryFileTracker->headWord;
            meanTracker = meanhead;
            // compare each word in the second file
            
            while(compareTracker != NULL) {
		printf("entered third loop (b) \n");
                // with each word in the first file
                while(meanTracker != NULL) {
			printf("entered fourth loop\n");
//			printf("mean tracker null? %d\n", meanTracker
//			if(meanTracker->name != NULL) 
//			printf("Comparing %s and %s\n", meanTracker->name, compareTracker->name);
    			if(meanTracker->name != NULL && strcmp(meanTracker->name, compareTracker->name) == 0) {
//			    printf("same name\n");
			    printf("%s and %s are same\n", meanTracker->name, compareTracker->name);
                        meanTracker->probability = meanTracker->probability + compareTracker->probability;
                        meanTracker->probability = meanTracker->probability/2;
                        meanTracker = meanhead;
                        break;
                    }
                    else if(meanTracker->nextWord == NULL) {
//			    printf("adding at end\n");
                        meanTracker->nextWord = (struct wordnode* ) malloc(sizeof(struct wordnode));
			meanTracker->nextWord->name = (char *) malloc(sizeof(char *) * (int) strlen(compareTracker->name));
                        strcpy(meanTracker->nextWord->name, compareTracker->name);
                        meanTracker->probability = compareTracker->probability/2;
			printf("%s was added at the end\n", meanTracker->nextWord->name);
                        meanTracker->nextWord->nextWord = NULL;
//			printf("all right\n");
                        meanTracker = meanhead;
                        break;
                    }
                    meanTracker = meanTracker->nextWord;

                }
                compareTracker = compareTracker->nextWord;
            }

            //find kld for first file
            compareTracker = fileTracker->headWord;
            double kld1 = 0.0;
            meanTracker = meanhead;
            while(compareTracker != NULL) {
//		printf("calculating kld1\n");
                while(meanTracker != NULL) {

                    if(strcmp(compareTracker->name, meanTracker->name) == 0) {
                        kld1 += ((compareTracker->probability) * log(compareTracker->probability/meanTracker->probability));
                        break;
                    }

                    meanTracker = meanTracker->nextWord;
                }

                compareTracker = compareTracker->nextWord;
            }

            //find kld for second file
            compareTracker = secondaryFileTracker->headWord;
            double kld2 = 0.0;
            meanTracker = meanhead;
            while(compareTracker != NULL) {
                while(meanTracker != NULL) {
                    if(strcmp(compareTracker->name, meanTracker->name) == 0) {
                        kld1 += ((compareTracker->probability) * log(compareTracker->probability/meanTracker->probability));
                        break;
                    }
                    meanTracker = meanTracker->nextWord;
                }
                compareTracker = compareTracker->nextWord;
            }

            //find jensen shennon and output
            double jsd = (kld1 + kld2)/2;

            if(jsd <= 0.1) {
                printf("\033[0;31m");
                printf("%f \"%s\" and \"%s\"\n", jsd, fileTracker->name, secondaryFileTracker->name);
            }
            else if(jsd > 0.1 && jsd <= 0.15) {
                printf("\033[0;33m");
                printf("%f \"%s\" and \"%s\"\n", jsd, fileTracker->name, secondaryFileTracker->name);
            }
            else if(jsd > 0.15 && jsd <= 0.2) {
                printf("\033[0;32m");
                printf("%f \"%s\" and \"%s\"\n", jsd, fileTracker->name, secondaryFileTracker->name);
            }
            else if(jsd > 0.2 && jsd <= 0.25) {
                printf("\033[0;36m");
                printf("%f \"%s\" and \"%s\"\n", jsd, fileTracker->name, secondaryFileTracker->name);
            }
            else if(jsd > 0.25 && jsd <= 0.3) {
                printf("\033[0;34m");
                printf("%f \"%s\" and \"%s\"\n", jsd, fileTracker->name, secondaryFileTracker->name);
            }
            else {
                printf("\033[0m");
                printf("%f \"%s\" and \"%s\"\n", jsd, fileTracker->name, secondaryFileTracker->name);
            }

            secondaryFileTracker = secondaryFileTracker->nextFile;
            if(secondaryFileTracker == NULL) 
                break;
        }        

        fileTracker = fileTracker->nextFile;
        if(fileTracker == NULL)
            break;
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
    char* buffercopy = (char *) malloc(sizeof(char) * filesize);
    bytes = read(fd, buffer, filesize);
    if(bytes < 0)
        printf("error\n");
    
    buffercopy = strcpy(buffercopy, buffer);
        
    char* token = strtok(buffercopy, " \n\t\v\f\r");
    int totalwords = 0;
    while(token != NULL)  {
        totalwords++;
        token = strtok(NULL, " \n\t\r\v\f"); 
    }

    funcArgs->wordcount = totalwords;
    
    token = strtok(buffer, " \n\t\v\f\r");
    printf("total words in token: %d\n", totalwords);

    while(token != NULL) {
        
        if(funcArgs->headWord == NULL) {
            struct wordnode headNode;
            headNode.name = token;
            headNode.totalCount = 1;
            headNode.probability = (double) 1/(double) totalwords;
            headNode.nextWord = NULL;

            funcArgs->headWord = (struct wordnode*) malloc(sizeof(struct wordnode));
            funcArgs->headWord = &headNode;
            token = strtok(NULL, " \n\t\r\v\f"); 
            continue;
        }

        struct wordnode* track;
        track = funcArgs->headWord;

        int found = 0;

        struct wordnode* tempNode = (struct wordnode*) malloc(sizeof(struct wordnode)); 
        tempNode->name = token;
        tempNode->totalCount = 1;
        tempNode->probability = (double) 1/(double) totalwords;
        tempNode->nextWord = NULL;

        while(track->nextWord != NULL) {

            if(strcmp(track->name, token) == 0) {
                track->totalCount = track->totalCount + 1;
                track->probability = (double) track->totalCount/(double) totalwords;
                found = 1;
                break;
            }

            track = track->nextWord;
            if(track->nextWord == NULL) {
                if(strcmp(track->name, token) == 0) {
                    track->totalCount = track->totalCount + 1;
                    track->probability = (double) track->totalCount/(double) totalwords;
                    found = 1;
                   // break;
                }
            }
        }

        if(found != 1) {
            track->nextWord = tempNode;
        }

        token = strtok(NULL, " \n\t\r\v\f"); 
    }

	struct wordnode* track;
       track = funcArgs->headWord;
       printf("File: %s\n", name);

	while(track != NULL) {
		printf("|_ Word: %s | Count: %d | Probability: %f _| ---> ", track->name, track->totalCount, 
        track->probability);
		track = track->nextWord;
	}
    printf("NULL\n");

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
                struct filenode* newNode = (struct filenode*) malloc(sizeof(struct filenode));
                if(headFile == NULL) {
                    newNode->name = (char *) malloc(sizeof(char *) * totalLen);
                    strcpy(newNode->name, tempName);
                    newNode->index = tracker;
                    headFile = newNode;
                    prevFile = newNode;
                }
                else {
                    newNode->name = (char *) malloc(sizeof(char *) * totalLen);
                    strcpy(newNode->name, tempName);
                    newNode->index = tracker;
                    prevFile->nextFile = newNode;
                    prevFile = newNode;
                }
              /*  files[tracker].name = (char *) malloc(sizeof(char *) * totalLen);
                strcpy(files[tracker].name, tempName);
                files[tracker].index = tracker;
		if(tracker != 0)
			files[tracker - 1].nextFile = &(files[tracker]); */
            //   files[tracker].trackerPtr = &tracker;
                pthread_create(allThreads+threadTracker, NULL, handleFile, newNode);
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
    if(dirp) {
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
                strcat(tempName, argv[1]);
                strcat(tempName, "/");
                strcat(tempName, dp->d_name);
                struct filenode* newNode = (struct filenode*) malloc(sizeof(struct filenode));
                if(headFile == NULL) {
                    newNode->name = (char *) malloc(sizeof(char *) * totalLen);
                    strcpy(newNode->name, tempName);
                    newNode->index = tracker;
                    headFile = newNode;
                    prevFile = newNode;
                }
                else {
                    newNode->name = (char *) malloc(sizeof(char *) * totalLen);
                    strcpy(newNode->name, tempName);
                    newNode->index = tracker;
                    prevFile->nextFile = newNode;
                    prevFile = newNode;
                }
        //       files[tracker].name = (char *) malloc(sizeof(char *) * totalLen);
        //     strcpy(files[tracker].name, tempName);
            //    files[tracker].index = tracker;
        // if(tracker != 0)
            //    files[tracker - 1].nextFile = &(files[tracker]);
            //   files[tracker].trackerPtr = &tracker;
                pthread_create(allThreads+threadTracker, NULL, handleFile, newNode);
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
                strcat(tempName, argv[1]);
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
    else if(ENOENT == errno) {
        printf("%s does not exist.\n", argv[1]);
        return EXIT_FAILURE;
    }
    else {
        printf("Opendir failed to open %s.\n", argv[1]);
        return EXIT_FAILURE;
    }
    int i = 0;
    for (i = 0; i <= threadTracker; ++i) {
        printf("Joining: %ld\n", allThreads[i]);
        pthread_join(allThreads[i], NULL);
    } 

    printf("Threads: %d\n", threadTracker);
    printf("Directories: %d\n", dirTracker);
    printf("Files: %d\n", tracker);
    
    //printFileArr();
    printf("Sorting files\n");
    sortWordsInFile();
    printf("Outputing jsd\n");
  	printFileArr();

    outputJensenShannon();
    closedir(dirp);
    
    return EXIT_SUCCESS;
}
