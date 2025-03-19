#include <stdio.h>
#include <setjmp.h>
#include <signal.h>
#include <stdlib.h>
#include <ucontext.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_THREAD 3
#define STACK_SIZE 16000
#define TIME_SLICE 4

void thread0();
void thread1();
void thread2();
void schedule(int sig);

ucontext_t uctx_main;
void (*thread_routine[MAX_THREAD])() = {thread0, thread1, thread2};
ucontext_t thread_save[MAX_THREAD];
char thread_stack[MAX_THREAD][STACK_SIZE];
int thread_state[MAX_THREAD];
int current;

void schedule(int sig) {
    int k, old;
    alarm(TIME_SLICE);  // reset the timer
    old = current;

    // Find next ready thread
    for (k = 0; k < MAX_THREAD; k++) {
        current = (current + 1) % MAX_THREAD;
        if (thread_state[current] == 1) break;
    }

    if (k == MAX_THREAD) {
        printf("last thread completed: exiting\n");
        exit(0);
    }

    printf("schedule: save(%d) restore(%d)\n", old, current);
    if (swapcontext(&thread_save[old], &thread_save[current]) == -1) {
        perror("swapcontext");
        exit(0);
    }
}

void thread0() {
    int i;
    for (i = 0; i < 5; i++) {
        printf("[Thread 0] iteration %d\n", i);
        sleep(1);
    }
    printf("[Thread 0] completed\n");
    thread_state[0] = 0;
    schedule(0);  // yield after completion
}

void thread1() {
    int i;
    for (i = 0; i < 5; i++) {
        printf("[Thread 1] iteration %d\n", i);
        sleep(1);
    }
    printf("[Thread 1] completed\n");
    thread_state[1] = 0;
    schedule(0);
}

void thread2() {
    int i;
    for (i = 0; i < 5; i++) {
        printf("[Thread 2] iteration %d\n", i);
        sleep(1);
    }
    printf("[Thread 2] completed\n");
    thread_state[2] = 0;
    schedule(0);
}

int main() {
    int i;

    // Create thread contexts
    for (i = 0; i < MAX_THREAD; i++) {
        if (getcontext(&thread_save[i]) == -1) {
            perror("getcontext");
            exit(0);
        }
        thread_save[i].uc_stack.ss_sp = thread_stack[i];
        thread_save[i].uc_stack.ss_size = sizeof(thread_stack[i]);
        thread_save[i].uc_link = &uctx_main;  // return to main when done

        makecontext(&thread_save[i], thread_routine[i], 0);
        thread_state[i] = 1;  // thread is ready
        printf("main: thread %d created\n", i);
    }

    signal(SIGALRM, schedule);
    alarm(TIME_SLICE);

    printf("main: swapcontext to thread 0\n");
    current = 0;

    if (swapcontext(&uctx_main, &thread_save[0]) == -1) {
        perror("swapcontext");
        exit(0);
    }

    // Shouldn't reach here unless threads return to uctx_main
    while (1) {
        printf("thread %d completed\n", current);
        thread_state[current] = 0;
        schedule(0);
    }

    return 0;
}
