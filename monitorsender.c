#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>

/*
This program simulates the reader-writer problem using a monitor. 
Multiple readers can read simultaneously but writers need exclusive access.
A monitor is implemented using mutex and condition variables in pthread.
*/

// The Monitor struct
typedef struct {
    int nbReaders;
    bool writer;
    pthread_mutex_t mutex;
    pthread_cond_t canRead;
    pthread_cond_t canWrite;
} Monitor;

// Global monitor instance
Monitor rwMonitor; // Reader-writer monitor

/*
init - Initializes the monitor
*/
void init() {
    rwMonitor.nbReaders = 0;
    rwMonitor.writer = false;
    pthread_mutex_init(&rwMonitor.mutex, NULL);
    pthread_cond_init(&rwMonitor.canRead, NULL);
    pthread_cond_init(&rwMonitor.canWrite, NULL);
}
void beginRead(){
    pthread_mutex_lock(&rwMonitor.mutex);
    while(rwMonitor.writer){
        pthread_cond_wait(&rwMonitor.canRead, &rwMonitor.mutex);
    }
    rwMonitor.nbReaders++;
    pthread_mutex_unlock(&rwMonitor.mutex);
}
void endRead(){
    pthread_mutex_lock(&rwMonitor.mutex);
    rwMonitor.nbReaders--;
    if(rwMonitor.nbReaders==0){
        pthread_cond_signal(&rwMonitor.canWrite);
    }
    pthread_mutex_unlock(&rwMonitor.mutex);
}
void beginWrite(){
    pthread_mutex_lock(&rwMonitor.mutex);
    while(rwMonitor.nbReaders>0 || rwMonitor.writer){
        pthread_cond_wait(&rwMonitor.canWrite, &rwMonitor.mutex);
    }
    rwMonitor.writer=true;
    pthread_mutex_unlock(&rwMonitor.mutex);
}
void endWrite(){
    pthread_mutex_lock(&rwMonitor.mutex);
    rwMonitor.writer=false;
    pthread_cond_signal(&rwMonitor.canWrite);    
    pthread_cond_signal(&rwMonitor.canRead);
    pthread_mutex_unlock(&rwMonitor.mutex);
}
/*
reader - Thread func for reader threads
*/
void *reader(void *arg){
    int id = *(int *)arg;
    while(1){
        sleep(rand() % 3); // wait for a random time before attempting to read
        printf("Reader %d is reading\n", id);

        beginRead(); // Start reading

        //reading critical section
        printf("Reader %d is reading(readers count: %d)\n", id, rwMonitor.nbReaders);
        sleep(1 + rand() % 2); // Simulate reading for 1-2 seconds
        endRead(); // Exit reading
        printf("Reader %d has finished reading\n", id);
    }
    return NULL;
}

/*
writer - Thread func for writer threads
*/
void *writer(void *arg){
    int id = *(int *)arg;
    while(1){
        sleep(rand() % 3); // wait for a random time before attempting to write
        printf("Writer %d is writing\n", id);

        beginWrite(); // Start writing

        //writing critical section
        printf("Writer %d is writing\n", id);
        sleep(1 + rand() % 2); // Simulate writing for 3-7 seconds
        endWrite(); // Exit writing
        printf("Writer %d has finished writing\n", id);
    }
    return NULL;
}

int main() {
    pthread_t readers[5], writers[3];
    int reader_ids[5] = {1, 2, 3, 4, 5};
    int writer_ids[3] = {1, 2, 3};

    // Initialize monitor
    init();

    // Seed random number generator
    srand(time(NULL));

    // Create reader threads
    for (int i = 0; i < 5; i++) {
        reader_ids[i] = i + 1;
        if(pthread_create(&readers[i], NULL, reader, &reader_ids[i]) != 0){
            perror("Error creating reader thread");
            exit(EXIT_FAILURE);
        }
    }

    // Create writer threads
    for (int i = 0; i < 3; i++){
        writer_ids[i] = i + 1;
        if(pthread_create(&writers[i], NULL, writer, &writer_ids[i]) != 0){
            perror("Error creating writer thread");
            exit(EXIT_FAILURE);
        }
    }

    printf("Stimulating the reader-writer problem...\n");
    sleep(10); 
    printf("Stimulating completed...\n");

    pthread_mutex_destroy(&rwMonitor.mutex);
    pthread_cond_destroy(&rwMonitor.canRead);
    pthread_cond_destroy(&rwMonitor.canWrite);

    return 0;
}
