#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>

#define MAX_FRAMES 4
#define DEFAUT_FRAME_COUNT 3

#define COLOR_RESET "\x1b[Om"
#define COLOR_RED "\x1b[31m"
#define COLOR_GREEN "\x1b[32m"

//Structure to represent a page in memory
typedef struct {
    int pageNO; //Page number
    int lastUsed; //Timestamp for aging technique
    bool isEmpty; //Flag to check if page is empty
} Page;

//Structure to hold the algorithm statistics
typedef struct {
    int pageFaults; //Number of page faults
    int pageHits; //Number of page hits
    float faultRate; //Fault rate
}Stats;

/*
Initializes the frames in memory
*/
void initFrames(Page frames[], int frameCount) {
    for (int i = 0; i < frameCount; i++) {
        frames[i].pageNO = -1;
        frames[i].lastUsed = -1;
        frames[i].isEmpty = true;
    }
}

/*
Find the index of the LRU page
*/
int findLRU(Page frames[], int frameCount) {
    int minLastUsed = frames[0].lastUsed;
    int lruIndex = 0;
    for (int i = 1; i < frameCount; i++) {
        if (frames[i].lastUsed < minLastUsed) {
            minLastUsed = frames[i].lastUsed;
            lruIndex = i;
        }
    }
    return lruIndex;
}

/*
Checks if a page is already in memory
*/
int findPage(Page frames[], int frameCount, int pageNO) {
    for (int i = 0; i < frameCount; i++) {
        if (!frames[i].isEmpty && frames[i].pageNO == pageNO) {
            return i;
        }
    }
    return -1;
}

/*
Finds the empty frame in memory
*/
int findEmptyFrame(Page frames[], int frameCount) {
    for (int i = 0; i < frameCount; i++) {
        if (frames[i].isEmpty) {
            return i;
        }
    }
    return -1;
}

/*
Display the current state of memory in a tabular format
*/
void displayMemoryState(Page frames[], int frameCount, int currentPage, bool isPageFault, int timestamp){
    printf("Time %d: Accessing Page %d\n", timestamp, currentPage);
    if(isPageFault){
        printf("%sPage Fault%s | " COLOR_RED, COLOR_RESET);
    } else {
        printf("%sPage Hit%s |" COLOR_GREEN ,COLOR_RESET);
    }
    printf(" Memory: [ ");
    for (int i = 0; i < frameCount; i++) {
        if (frames[i].isEmpty) {
            printf("-- ");
        } else if(frames[i].pageNO == currentPage){
            printf("%s%2d %s", COLOR_GREEN, frames[i].pageNO, COLOR_RESET);
        }
        else {
            printf("%2d ", COLOR_RED, frames[i].pageNO, COLOR_RESET);
        }
    }
    printf("] | LRU Order: [ ");
    Page tempFrames[MAX_FRAMES];
    memcpy(tempFrames, frames, frameCount * sizeof(Page));
    for (int i=0; i<frameCount; i++){
        for(int j=0; j<frameCount-i-1; j++){
            if(!tempFrames[j].isEmpty && !tempFrames[j+1].isEmpty && tempFrames[j].lastUsed > tempFrames[j+1].lastUsed){
                Page temp = tempFrames[j];
                tempFrames[j] = tempFrames[j+1];
                tempFrames[j+1] = temp;
            }
        }
    }
    for (int i = 0; i < frameCount; i++) {
        if(tempFrames[i].isEmpty && !tempFrames[i+1].isEmpty){
            printf("-- ");
        } 
        else {
            printf("%2d ", tempFrames[i].pageNO);
        }
    }
    printf("\n");
}
/*
Simulates the LRU page replacement algorithm
*/
Stats simulateLRU(int refStr[], int size, int frameCount, bool verbose){
    Page frames[MAX_FRAMES];
    Stats stats = {0, 0, 0.0};

    if (frameCount > MAX_FRAMES) {
        printf("Warning: Requested frame count exceeds the maximum limit");
        frameCount = MAX_FRAMES;
    }

    initFrames(frames, frameCount);
    for (int i = 0; i < size; i++){
        int currentPage = refStr[i];
        bool pageFault = false;
        int pageIndex = findPage(frames, frameCount, currentPage);
        if (pageIndex == -1) {
            pageFault = true;
            stats.pageFaults++;
            int emptyFrameIndex = findEmptyFrame(frames, frameCount);
            if (emptyFrameIndex != -1) {
                frames[emptyFrameIndex].pageNO = currentPage;
                frames[emptyFrameIndex].isEmpty = false;
                frames[emptyFrameIndex].lastUsed = i;
            }
            else {
                int lruIndex = findLRU(frames, frameCount);
                frames[lruIndex].pageNO = currentPage;
                frames[lruIndex].lastUsed = i;
                frames[lruIndex].isEmpty = false;
            }
        } 
        else {
            stats.pageHits++;
            frames[pageIndex].lastUsed = i;
        }

        if(verbose){
            displayMemoryState(frames, frameCount, currentPage, pageFault, i);
        }
    }
    stats.faultRate = (float)stats.pageFaults / size * 100;
    return stats;
}

int main(int argc, char *argv[]) {
    int defautRefString[] = {0,1,2,3,0,1,4,0,1,2,3,4,1,2};
    int *referenceString = defautRefString;
    int size = sizeof(defautRefString) / sizeof(defautRefString[0]);
    int frameCount = DEFAUT_FRAME_COUNT;
    bool verbose = false;

    for(int i=1; i<argc; i++){
        if(strcmp(argv[i], "-f") == 0 && i+1 < argc){
            frameCount = atoi(argv[i+1]);
            if (frameCount <= 0){
                printf("Error: Frame count must be positive\n");
                return 1;
            }
            i++;
        } else if(strcmp(argv[i], "-q") == 0){
            verbose = false;
        }   
    }
    printf("LRU stimulation");
    printf("Reference String: ");
    for (int i = 0; i < size; i++) {
        printf("%d ", referenceString[i]);
    }
    printf("\n\n");
    Stats stats = simulateLRU(referenceString, size, frameCount, verbose);

    return 0;
}