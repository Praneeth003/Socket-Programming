#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <semaphore.h>

char *str;
sem_t sem0, sem1, sem2, sem3, sem4;

void fun(char *inp, char ch)
{
    int i = 0;
    int n = strlen(inp);
    for (i = 0; i < strlen(inp); i++)
    {
        if (inp[i] == ch)
        {
            inp[i] = toupper(ch);
        }
    }
    // printf("%s \n", str);
}

void *charA(void *args)
{
    sem_wait(&sem0);
    char *inp;
    inp = (char *)args;
    printf("Input string: %s \n", inp);
    printf("Capitalizing a \n");
    fun(inp, 'a');
    printf("Output string: %s \n", inp);

    sem_post(&sem1);
    pthread_exit(NULL);
}

void *charE(void *args)
{
    sem_wait(&sem1);
    char *inp;
    inp = (char *)args;
    printf("Input string: %s \n", inp);
    printf("Capitalizing e \n");
    fun(inp, 'e');
    printf("Output string: %s \n", inp);

    sem_post(&sem2);
    pthread_exit(NULL);
}

void *charI(void *args)
{
    sem_wait(&sem2);
    char *inp;
    inp = (char *)args;
    printf("Input string: %s \n", inp);
    printf("Capitalizing i \n");
    fun(inp, 'i');
    printf("Output string: %s \n", inp);

    sem_post(&sem3);
    pthread_exit(NULL);
}

void *charO(void *args)
{
    sem_wait(&sem3);
    char *inp;
    inp = (char *)args;
    printf("Input string: %s \n", inp);
    printf("Capitalizing o \n");
    fun(inp, 'o');
    printf("Output string: %s \n", inp);

    sem_post(&sem4);
    pthread_exit(NULL);
}

void *charU(void *args)
{
    sem_wait(&sem4);
    char *inp;
    inp = (char *)args;
    printf("Input string: %s \n", inp);
    printf("Capitalizing u \n");
    fun(inp, 'u');
    printf("Output string: %s \n", inp);
    pthread_exit(NULL);
}

int main()
{
    sem_init(&sem0, 0, 1);
    sem_init(&sem1, 0, 0);
    sem_init(&sem2, 0, 0);
    sem_init(&sem3, 0, 0);
    sem_init(&sem4, 0, 0);
    
    pthread_t thread_id1;
    pthread_t thread_id2;
    pthread_t thread_id3;
    pthread_t thread_id4;
    pthread_t thread_id5;

    str = malloc(1000);
    strcpy(str, "aeiouHELLAEIOU");

    char *input = malloc(1000);
    strcpy(input, str);
 
    pthread_create(&thread_id1, NULL, charA, (void *)input);
    pthread_create(&thread_id2, NULL, charE, (void *)input);
    pthread_create(&thread_id3, NULL, charI, (void *)input);
    pthread_create(&thread_id4, NULL, charO, (void *)input);
    pthread_create(&thread_id5, NULL, charU, (void *)input);

    pthread_join(thread_id1, NULL);
    pthread_join(thread_id2, NULL);
    pthread_join(thread_id3, NULL);
    pthread_join(thread_id4, NULL);
    pthread_join(thread_id5, NULL);

    sem_destroy(&sem0);
    sem_destroy(&sem1);
    sem_destroy(&sem2);
    sem_destroy(&sem3);
    sem_destroy(&sem4);



    // printf("\nFinal string is:%s\n", input);

    // exit(0);
    // free(input);
    // free(str);
    return 0;
}
