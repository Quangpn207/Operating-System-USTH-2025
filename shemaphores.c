#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<semaphore.h>
#include<unistd.h>
#include<time.h>

/*
This program simulates the reader-writer problem using semaphores.
Multiple readers can read simultaneously but writers need exclusive access.
*/


// Shared data
int nbReaders = 0; // Counter for active readers
sem_t mutex;     // Semaphore for protecting access to nbReaders
sem_t blockwriter; // Semaphore for blocking writers (called "exclusive")


//function prototypes
void beginRead();    //Protocal for starting a read operation
void endRead();      //Protocal for ending a read operation
void beginWrite();   //Protocal for starting a write operation
void endWrite();     //Protocal for ending a write operation
void *reader(void *arg); // Thread func for reader thread
void *writer(void *arg); // Thread func for writer thread

void beginRead(){
    sem_wait(&mutex); // Lock mutex to update nbReaders safely
    if(nbReaders == 0) // If this is the first reader
        sem_wait(&blockwriter); // Block any protential writers
    nbReaders++; // Increment the reader count
    sem_post(&mutex); // Release mutex
}

void endRead(){
    sem_wait(&mutex);
    nbReaders--; // Decrement the reader count  
    if(nbReaders == 0) // If this was the last reader
        sem_post(&blockwriter); // Unblock any waiting writers
    sem_post(&mutex); // Release mutex
}

void beginWrite(){
    sem_wait(&blockwriter); // Block any readers or writers (waits for exclusive access)
}

void endWrite(){
    sem_post(&blockwriter); // Release exclusive access
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
        printf("Reader %d is reading(readers count: %d)\n", id, nbReaders);
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

    // Initialize semaphores
    sem_init(&mutex, 0, 1);
    sem_init(&blockwriter, 0, 1);

    // Seed random number generator
    srand(time(NULL));

    // Create reader threads
    for (int i = 0; i < 5; i++) {
        reader_ids[i] = i + 1;
        if(pthread_create(&readers[i], NULL, reader, &reader_ids[i])!=0){
            perror("Error creating reader thread");
            exit(EXIT_FAILURE);
        }
    }

    // Create writer threads
    for (int i = 0; i < 3; i++){
        writer_ids[i] = i + 1;
        if(pthread_create(&writers[i], NULL, writer, &writer_ids[i])!=0){
            perror("Error creating writer thread");
            exit(EXIT_FAILURE);
        }
    }

    printf("Stimulating the reader-writer problem...\n");
    sleep(10); 
    printf("Stimulating completed...\n");

    sem_destroy(&mutex);
    sem_destroy(&blockwriter);

    return 0;
}
