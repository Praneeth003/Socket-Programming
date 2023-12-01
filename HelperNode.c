#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <semaphore.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <errno.h>
#include <arpa/inet.h>

sem_t sem0, sem1, sem2, sem3, sem4;

void capitalize(char *inp, char ch)
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
}

void *charA(void *args)
{
    sem_wait(&sem0);
    char *inp;
    inp = (char *)args;
    printf("Input string: %s \n", inp);
    printf("Capitalizing a \n");
    capitalize(inp, 'a');
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
    capitalize(inp, 'e');
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
    capitalize(inp, 'i');
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
    capitalize(inp, 'o');
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
    capitalize(inp, 'u');
    printf("Output string: %s \n", inp);
    pthread_exit(NULL);
}

int main(int argc, char const *argv[])
{
    sem_init(&sem0, 0, 1);
    sem_init(&sem1, 0, 0);
    sem_init(&sem2, 0, 0);
    sem_init(&sem3, 0, 0);
    sem_init(&sem4, 0, 0);
    pthread_t thread1, thread2, thread3, thread4, thread5;

    int PORT, i, maxfd, activity;
    printf("Enter Port Number:");
    scanf("%d", &PORT);
    char *str, *inp;
    sem_t sem0, sem1, sem2, sem3, sem4;

    // Socket File descriptor
    int helpersockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (helpersockfd < 0)
    {
        perror("Socket Failed");
        return -1;
    }

    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &address.sin_addr) <= 0)
    {
        printf("\n Invalid address or Address not supported \n");
        return -1;
    }

    socklen_t addresslen = sizeof(address);
    int status = connect(helpersockfd, (struct sockaddr *)&address, addresslen);
    if (status < 0)
    {
        perror("connect");
        return -1;
    }
    printf("Connected to server\n");

    ssize_t valread;
    char buffer[1024];

    while (1)
    {
        memset(buffer, 0, sizeof(buffer));
        valread = recv(helpersockfd, buffer, 1023, 0);
        if (valread < 0)
        {
            perror("Receiving Error");
        }
        buffer[valread] = '\0';
        if (valread == 0)
        {
            printf("Server Disconnected\n");
            break;
        }
        else
        {
            printf("Received Message: %s \n", buffer);
            char *input = malloc(1000);
            strcpy(input, buffer);

            pthread_create(&thread1, NULL, charA, (void *)input);
            pthread_create(&thread2, NULL, charE, (void *)input);
            pthread_create(&thread3, NULL, charI, (void *)input);
            pthread_create(&thread4, NULL, charO, (void *)input);
            pthread_create(&thread5, NULL, charU, (void *)input);

            pthread_join(thread1, NULL);
            pthread_join(thread2, NULL);
            pthread_join(thread3, NULL);
            pthread_join(thread4, NULL);
            pthread_join(thread5, NULL);

            sem_destroy(&sem0);
            sem_destroy(&sem1);
            sem_destroy(&sem2);
            sem_destroy(&sem3);
            sem_destroy(&sem4);

            send(helpersockfd, input, strlen(input), 0);
        }
    }
}