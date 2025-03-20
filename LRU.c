#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// Configurable parameters
#define MAX_FRAMES 4
#define DEFAULT_FRAME_COUNT 4

// ANSI color codes for better visualization
#define COLOR_RESET "\x1B[0m"
#define COLOR_RED "\x1B[31m"
#define COLOR_GREEN "\x1B[32m"

// Structure to represent a page in memory
typedef struct
{
    int pageNo;   // Page number
    int lastUsed; // Timestamp for aging technique
    bool isEmpty; // Flag to check if frame is empty
} Page;

// Structure to hold algorithm statistics
typedef struct
{
    int pageFaults;  // Numbers of page faults
    int pageHits;    // Numbers of page hits
    float faultRate; // Page fault rate
} Stats;

/*
    Initialize the frames in memory
*/
void initializeFrames(Page frames[], int frameCount)
{
    for (int i = 0; i < frameCount; i++)
    {
        frames[i].pageNo = -1;
        frames[i].lastUsed = -1;
        frames[i].isEmpty = true;
    }
}

/*
    Find the index of the LRU page
*/
int findLRU(Page frames[], int frameCount)
{
    int minLastUsed = frames[0].lastUsed;
    int lruIndex = 0;

    for (int i = 1; i < frameCount; i++)
    {
        if (frames[i].lastUsed < minLastUsed)
        {
            minLastUsed = frames[i].lastUsed;
            lruIndex = i;
        }
    }

    return lruIndex;
}

/*
    Checks if a page is already in memory
*/
int findPage(Page frames[], int frameCount, int pageNo)
{
    for (int i = 0; i < frameCount; i++)
    {
        if (!frames[i].isEmpty && frames[i].pageNo == pageNo)
        {
            return i;
        }
    }
    return -1; // Page not found
}

/*
    Finds the empty frame in memory
*/
int findEmptyFrame(Page frames[], int frameCount)
{
    for (int i = 0; i < frameCount; i++)
    {
        if (frames[i].isEmpty)
            return i;
    }
    return -1;
}

/*
    Display the current state of memory in a tabular format
*/
void displayMemoryState(Page frames[], int frameCount, int currentPage, bool isPageFault, int timestamp)
{
    printf("Time %2d: Accessing Page %2d | ", timestamp, currentPage);

    if (isPageFault)
    {
        printf("%sPage Fault%s | ", COLOR_RED, COLOR_RESET);
    }
    else
    {
        printf("%sPage Hit%s | ", COLOR_GREEN, COLOR_RESET);
    }

    printf("Memory: [");
    for (int i = 0; i < frameCount; i++)
    {
        if (frames[i].isEmpty)
        {
            printf("--");
        }
        else if (frames[i].pageNo == currentPage)
        {
            printf("%s%2d %s", COLOR_GREEN, frames[i].pageNo, COLOR_RESET);
        }
        else
        {
            printf("%2d ", frames[i].pageNo);
        }
    }
    printf("] | LRU Order: [ ");

    Page tempFrames[MAX_FRAMES];
    memcpy(tempFrames, frames, frameCount * sizeof(Page));
    for (int i = 0; i < frameCount; i++)
    {
        for (int j = 0; j < frameCount - i - 1; i++)
        {
            if (!tempFrames[j].isEmpty && !tempFrames[j + 1].isEmpty &&
                tempFrames[j].lastUsed > tempFrames[j + 1].lastUsed)
            {
                Page temp = tempFrames[j];
                tempFrames[j] = tempFrames[j + 1];
                tempFrames[j + 1] = temp;
            }
        }
    }

    for (int i = 0; i < frameCount; i++)
    {
        if (tempFrames[i].isEmpty)
        {
            printf("-- ");
        }
        else
        {
            printf("%2d ", tempFrames[i].pageNo);
        }
    }
    printf(" ]\n");
}
/*
    Simulates the LRU algorithm
*/
Stats simulateLRU(int refSTR[], int size, int frameCount, bool verbose)
{
    Page frames[MAX_FRAMES];
    Stats stats = {0, 0, 0.0};

    if (frameCount > MAX_FRAMES)
    {
        printf("Warning: Requested frame count exceeds maximum\n");
        frameCount = MAX_FRAMES;
    }

    initializeFrames(frames, frameCount);

    for (int i = 0; i < size; i++)
    {
        int currentPage = refSTR[i];
        bool pageFault = false;

        int pageIndex = findPage(frames, frameCount, currentPage);

        if (pageIndex == -1)
        {
            pageFault = true;
            stats.pageFaults++;

            int emptyFrameIndex = findEmptyFrame(frames, frameCount);

            if (emptyFrameIndex != -1)
            {
                // Empty slot available
                frames[emptyFrameIndex].pageNo = currentPage;
                frames[emptyFrameIndex].isEmpty = false;
                frames[emptyFrameIndex].lastUsed = i;
            }
            else
            {
                // Replacement happens
                int lruIndex = findLRU(frames, frameCount);
                frames[lruIndex].pageNo = currentPage;
                frames[lruIndex].lastUsed = i;
            }
        }
        else
        {
            stats.pageHits++;
            frames[pageIndex].lastUsed = i;
        }

        if (verbose)
        {
            displayMemoryState(frames, frameCount, currentPage, pageFault, i);
        }
    }
    stats.faultRate = (float)stats.pageFaults / size * 100;

    return stats;
}

int main(int argc, char *argv[])
{
    int defaultRefStr[] = {0, 1, 2, 3, 0, 1, 4, 0, 1, 2, 3, 4, 1, 2};
    int *referStr = defaultRefStr;
    int size = sizeof(defaultRefStr) / sizeof(defaultRefStr[0]);
    int frameCount = DEFAULT_FRAME_COUNT;
    bool verbose = true;

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-f") == 0 && i + 1 < argc)
        {
            frameCount = atoi(argv[i + 1]);
            if (frameCount <= 0)
            {
                printf("Error: Frame count must be positive\n");
                return 1;
            }
            i++;
        }
        else if (strcmp(argv[i], "-q") == 0)
        {
            verbose = false;
        }
    }

    printf("LRU Simulation\n");
    printf("Ref String: ");
    for (int i = 0; i < size; i++)
    {
        printf("%d ", referStr[i]);
    }
    printf("\n\n");

    Stats stats = simulateLRU(referStr, size, frameCount, verbose);
    printf("%d %d %.2f\n",stats.pageFaults, stats.pageHits, stats.faultRate);
    return 0;
}