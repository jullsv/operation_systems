#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct {
    int event_id;
} EventData;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;

int ready = 0; 

void* data_ptr = NULL; 

void* provider_func(void* arg) {
    for (int i = 0; i < 5; i++) {
        sleep(1);

        pthread_mutex_lock(&lock);

        EventData* new_data = (EventData*)malloc(sizeof(EventData));
        if (new_data == NULL) {
            pthread_mutex_unlock(&lock);
            return NULL; 
        }
        new_data->event_id = i + 1;

        data_ptr = (void*)new_data; 
        ready = 1; 
        
        printf("provided №%d\n", new_data->event_id);

        pthread_cond_signal(&cond1);

        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

void* consumer_func(void* arg) {
    for (int i = 0; i < 5; i++) {
        pthread_mutex_lock(&lock);

        while (ready == 0) {
            pthread_cond_wait(&cond1, &lock);
        }
        
        EventData* data_to_process = (EventData*)data_ptr;

        ready = 0;
        data_ptr = NULL; 

        printf("consumed №%d\n", data_to_process->event_id);

        free(data_to_process);

        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

int main() {
    pthread_t provider_thread, consumer_thread;

    pthread_create(&provider_thread, NULL, provider_func, NULL);
    pthread_create(&consumer_thread, NULL, consumer_func, NULL);

    pthread_join(provider_thread, NULL);
    pthread_join(consumer_thread, NULL);

    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&cond1);

    printf("All is good! The end)\n");
    return 0;
}