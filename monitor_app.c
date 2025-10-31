#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;
int ready = 0;

void* provider_func(void* arg) {
    for (int i = 0; i < 5; i++) {
        sleep(1);

        pthread_mutex_lock(&lock);

        ready = 1;
        printf("Поставщик: Событие #%d инициировано.\n", i + 1);

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
        ready = 0;
        printf("Потребитель: Событие #%d получено и обработано.\n", i + 1);

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

    printf("Все события обработаны. Завершение программы.\n");
    return 0;
}