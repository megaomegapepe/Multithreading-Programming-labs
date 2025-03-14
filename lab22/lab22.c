#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <signal.h>
#include <semaphore.h>

sem_t a, b, c, module;
int widget_count = 0;

void exit_error(int error_num, const char *msg)
{
    char buf[256];
    strerror_r(error_num, buf, sizeof buf);
    fprintf(stderr, "Error: %s\n", buf);
    exit(EXIT_FAILURE);
}

void *a_routine(void *arg)
{
    while (1)
    {
        sleep(1);
        sem_post(&a);
        printf("A was created!\n");
    }
}

void *b_routine(void *arg)
{
    while (1)
    {
        sleep(2);
        sem_post(&b);
        printf("B was created!\n");
    }
}

void *c_routine(void *arg)
{
    while (1)
    {
        sleep(3);
        sem_post(&c);
        printf("C was created!\n");
    }
}

void *module_routine(void *arg)
{
    while (1)
    {
        sem_wait(&b);
        sem_wait(&a);
        sem_post(&module);
        printf("Module was created!\n");
    }
}

void *widget_routine(void *arg)
{
    while (1)
    {
        sem_wait(&c);
        sem_wait(&module);
        widget_count++;
        printf("!!! Widget was created !!!\n");
    }
}

int main(int argc, char *argv[])
{
    pthread_t thread_a, thread_b, thread_c, thread_module, thread_widget;
    int error;
    int a_count = 0, b_count = 0, c_count = 0, module_count = 0;

    error = sem_init(&a, 0, 0);
    if (error)
    {
        exit_error(error, "semaphore initialize failed");
    }
    error = sem_init(&b, 0, 0);
    if (error)
    {
        exit_error(error, "semaphore initialize failed");
    }
    error = sem_init(&c, 0, 0);
    if (error)
    {
        exit_error(error, "semaphore initialize failed");
    }
    error = sem_init(&module, 0, 0);
    if (error)
    {
        exit_error(error, "semaphore initialize failed");
    }

    sigset_t set, oset;
    sigemptyset(&set);
    sigemptyset(&oset);

    sigaddset(&set, SIGINT);
    pthread_sigmask(SIG_BLOCK, &set, &oset);

    error = pthread_create(&thread_a, NULL, a_routine, NULL);
    if (error)
        exit_error(error, "pthread_create failed");
    error = pthread_create(&thread_b, NULL, b_routine, NULL);
    if (error)
        exit_error(error, "pthread_create failed");
    error = pthread_create(&thread_c, NULL, c_routine, NULL);
    if (error)
        exit_error(error, "pthread_create failed");
    error = pthread_create(&thread_module, NULL, module_routine, NULL);
    if (error)
        exit_error(error, "pthread_create failed");
    error = pthread_create(&thread_widget, NULL, widget_routine, NULL);
    if (error)
        exit_error(error, "pthread_create failed for thread_a");

    pthread_sigmask(SIG_UNBLOCK, &set, &oset);

    sigwait(&set);

    sem_getvalue(&a, &a_count);
    sem_getvalue(&b, &b_count);
    sem_getvalue(&c, &c_count);
    sem_getvalue(&module, &module_count);

    char buf[256];
    error = pthread_cancel(thread_a);
    if (error)
    {
        strerror_r(error, buf, sizeof buf);
        fprintf(stderr, "Error: %s\n", buf);
    }
    error = pthread_cancel(thread_b);
    if (error)
    {
        strerror_r(error, buf, sizeof buf);
        fprintf(stderr, "Error: %s\n", buf);
    }
    error = pthread_cancel(thread_c);
    if (error)
    {
        strerror_r(error, buf, sizeof buf);
        fprintf(stderr, "Error: %s\n", buf);
    }
    error = pthread_cancel(thread_module);
    if (error)
    {
        strerror_r(error, buf, sizeof buf);
        fprintf(stderr, "Error: %s\n", buf);
    }
    error = pthread_cancel(thread_widget);
    if (error)
    {
        strerror_r(error, buf, sizeof buf);
        fprintf(stderr, "Error: %s\n", buf);
    }

    error = pthread_join(thread_a, NULL);
    if (error)
    {
        strerror_r(error, buf, sizeof buf);
        fprintf(stderr, "Error: %s\n", buf);
    }
    error = pthread_join(thread_b, NULL);
    if (error)
    {
        strerror_r(error, buf, sizeof buf);
        fprintf(stderr, "Error: %s\n", buf);
    }
    error = pthread_join(thread_c, NULL);
    if (error)
    {
        strerror_r(error, buf, sizeof buf);
        fprintf(stderr, "Error: %s\n", buf);
    }
    error = pthread_join(thread_module, NULL);
    if (error)
    {
        strerror_r(error, buf, sizeof buf);
        fprintf(stderr, "Error: %s\n", buf);
    }
    error = pthread_join(thread_widget, NULL);
    if (error)
    {
        strerror_r(error, buf, sizeof buf);
        fprintf(stderr, "Error: %s\n", buf);
    }
    printf("\nA count: %d\nB count: %d\nC count: %d\nModules count: %d\nWidget count: %d\n", a_count, b_count, c_count, module_count, widget_count);

    //Semaphores destroy
    error = sem_destroy(&a);
    if (error)
    {
        strerror_r(error, buf, sizeof buf);
        fprintf(stderr, "Error: %s\n", buf);
    }
    error = sem_destroy(&b);
    if (error)
    {
        strerror_r(error, buf, sizeof buf);
        fprintf(stderr, "Error: %s\n", buf);
    }
    error = sem_destroy(&c);
    if (error)
    {
        strerror_r(error, buf, sizeof buf);
        fprintf(stderr, "Error: %s\n", buf);
    }
    error = sem_destroy(&module);
    if (error)
    {
        strerror_r(error, buf, sizeof buf);
        fprintf(stderr, "Error: %s\n", buf);
    }

    pthread_exit(NULL);
    return 0;
}

#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>

sem_t isEmpty;
sem_t isFull;
sem_t global;

struct Message{
    char message[81];
    struct Message* next;
    struct Message* prev;
};

struct Queue{
    struct Message* head;
    struct Message* tail;
    int isDroped;
};

void mymsginit(struct Queue* queue){
    sem_init(&isEmpty, 0, 10);
    sem_init(&isFull, 0, 0);
    sem_init(&global,0 , 1);

    queue->head = NULL;
    queue->tail = NULL;
    queue->isDroped = 0;
}

void mymsgdrop(struct Queue* queue){
    struct Message* tmp;
    queue->isDroped = 1;
    sem_wait(&global);
    sem_post(&isEmpty);
    sem_post(&isFull);
    tmp = queue->head;
    while(tmp){
        struct Message* tmp1;
        tmp1 = tmp->next;
        free(tmp);
        tmp = tmp1;
    }
    sem_post(&global);
}

void mymsgdestroy(struct Queue* queue){
    sem_destroy(&global);
    sem_destroy(&isEmpty);
    sem_destroy(&isFull);
}

int mymsgget(struct Queue* queue, char* buf, size_t bufSize){
    struct Message* tmp;

    sem_wait(&isFull);
    sem_wait(&global);


    if (queue->isDroped == 1){
        sem_post(&global);
        sem_post(&isFull);
        return 0;
    }

    tmp = queue->tail;
    if (queue->head == tmp){
        queue->head = NULL;
        queue->tail = NULL;
    } else {
        queue->tail = tmp->prev;
        queue->tail->next = NULL;
    }
    //sem_post(&global);
    strncpy(buf, tmp->message, bufSize - 1);
    buf[bufSize - 1] = '\0';
    free(tmp);
    //sem_post(&isEmpty);
    return strlen(buf) + 1;
}

int mymsgput(struct Queue* queue, char* msg){
    struct Message* new_msg;

    sem_wait(&isEmpty);
    sem_wait(&global);

    if (queue->isDroped == 1){
        sem_post(&global);
        sem_post(&isEmpty);
        return 0;
    }

    new_msg = (struct Message*) malloc(sizeof(struct Message));
    strncpy(new_msg->message, msg, sizeof(new_msg->message) - 1);
    new_msg->message[sizeof(new_msg->message) - 1] = '\0';
    new_msg->prev = NULL;
    new_msg->next = queue->head;

    if (queue->head == NULL){
        queue->head = new_msg;
        queue->tail = new_msg;
    } else {
        queue->head->prev = new_msg;
        queue->head = new_msg;
    }

    sem_post(&global);
    sem_post(&isFull);
    return strlen(new_msg->message) + 1;
}

int flagQuit = 0;

void* producer(void* arg){
    struct Queue* queue = (struct Queue*) arg;
    int i = 0;

    for (i; i < 1000; i++){
        if (flagQuit == 1) break;
        char buf[40];
        sprintf(buf, "Message %d from thread %d", i, pthread_self());
        if (mymsgput(queue, buf) == 0){
            pthread_exit(NULL);
        }
    }
    pthread_exit(NULL);
}

void* consumer(void *arg){
    struct Queue* queue = (struct Queue*) arg;
    int length;
    while(1){
        if (flagQuit == 1) break;
        char buf[41];
        length = mymsgget(queue, buf, sizeof(buf));
        if (length == 0){
            break;
        }
        else {
            printf("Received by thread %d: %s\n", pthread_self(), buf);
        }
        sem_post(&global);
        sem_post(&isEmpty);
    }
    pthread_exit(NULL);
}


void print_error(int error_num){
    char buf[256];
    strerror_r(error_num, buf, sizeof(buf));
    fprintf(stderr,  buf);
}

int main(int argc, char* argv[]){
    struct Queue queue;
    int nProducers, nConsumers;
    pthread_t *producers, *consumers;
    int i;

    if (argc < 2){
        fprintf(stderr, "Requires arguments\n");
        exit(EXIT_FAILURE);
    }

    nProducers = atoi(argv[1]);
    nConsumers = atoi(argv[2]);

    if (nProducers == 0 || nConsumers == 0){
        fprintf(stderr, "Invalid arguments\n");
        exit(EXIT_FAILURE);
    }
    producers = (pthread_t*)malloc(sizeof(pthread_t*) * nProducers);
    consumers = (pthread_t*)malloc(sizeof(pthread_t*) * nConsumers);

    if (!producers || !consumers){
        fprintf(stderr, "Failed to allocate memory for threads\n");
        exit(EXIT_FAILURE);
    }

    int error_num;
    mymsginit(&queue);

    sigset_t signalSet;
    sigemptyset(&signalSet);
    sigaddset(&signalSet, SIGINT);
    pthread_sigmask(SIG_BLOCK, &signalSet, NULL);

    for (i = 0; i < nProducers || i < nConsumers; i++){
        if (i < nProducers){
            error_num = pthread_create(&producers[i], NULL, producer, &queue);
            if (error_num != 0){
                print_error(error_num);
                exit(EXIT_FAILURE);
            }
        }
        if (i < nConsumers){
            error_num = pthread_create(&consumers[i], NULL, consumer, &queue);
            if (error_num != 0){
                print_error(error_num);
                exit(EXIT_FAILURE);
            }
        }
    }

    //sleep(10);


    pthread_sigmask(SIG_UNBLOCK, &signalSet, NULL);
    int sig;
    sig = sigwait(&signalSet);
    if (sig == SIGINT) flagQuit = 1;


    mymsgdrop(&queue);

    for (i = 0; i < nProducers || i < nConsumers; i++){
        if (i < nProducers) {
            error_num = pthread_join(producers[i], NULL);
            if (error_num != 0){
                print_error(error_num);
                exit(EXIT_FAILURE);
            }
        }
        if (i < nConsumers){
            error_num = pthread_join(consumers[i], NULL);
            if (error_num != 0){
                print_error(error_num);
                exit(EXIT_FAILURE);
            }
        }
    }

    mymsgdestroy(&queue);

    printf("\nAll threads quit and queue destroyed\n");

    free(producers);
    free(consumers);

    pthread_exit(NULL);
    return(EXIT_SUCCESS);

}