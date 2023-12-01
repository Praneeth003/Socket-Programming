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

sem_t sem0, sem1, sem2, sem3, sem4, sem5, sem6, sem7;
char *str, *inp;
char input[1024];

// Function to capitalize the given character
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

// Function to create a thread for serverEncoder
void *serverEncoder(void *args)
{
    // passes the string back to the socket
    sem_wait(&sem7);
    char *inp;
    inp = (char *)args;
}

// Function to create a thread to capitalize 'a'
void *charA(void *args)
{
    sem_wait(&sem0);
    char *inp;
    inp = (char *)args;
    capitalize(inp, 'a');
    sem_post(&sem1);
    pthread_exit(NULL);
}

// Function to create a thread to capitalize 'e'
void *charE(void *args)
{
    sem_wait(&sem1);
    char *inp;
    inp = (char *)args;
    capitalize(inp, 'e');
    sem_post(&sem2);
    pthread_exit(NULL);
}

// Function to create a thread to capitalize 'i'
void *charI(void *args)
{
    sem_wait(&sem2);
    char *inp;
    inp = (char *)args;
    capitalize(inp, 'i');
    sem_post(&sem3);
    pthread_exit(NULL);
}

// Function to create a thread to capitalize 'o'
void *charO(void *args)
{
    sem_wait(&sem3);
    char *inp;
    inp = (char *)args;
    capitalize(inp, 'o');
    sem_post(&sem4);
    pthread_exit(NULL);
}

// Function to create a thread to capitalize 'u'
void *charU(void *args)
{
    sem_wait(&sem4);
    char *inp;
    inp = (char *)args;
    capitalize(inp, 'u');
    sem_post(&sem5);
    pthread_exit(NULL);
}

// Function to create a thread to write the string to the ServerEncoder thread
void *writer(void *args)
{
    sem_wait(&sem5);
    char *inp;
    inp = (char *)args;
    sem_init(&sem7, 0, 0);
    pthread_t threadEnc;
    pthread_create(&threadEnc, NULL, serverEncoder, (void *)inp);
    pthread_join(threadEnc, NULL);
    sem_destroy(&sem6);
    sem_post(&sem7);
    pthread_exit(NULL);
}

// Function to create a thread to read the string from the socket
void *serverDecoder(void *args)
{
    sem_wait(&sem6);
    sem_init(&sem0, 0, 1);
    sem_init(&sem1, 0, 0);
    sem_init(&sem2, 0, 0);
    sem_init(&sem3, 0, 0);
    sem_init(&sem4, 0, 0);
    sem_init(&sem5, 0, 0);
    pthread_t thread1, thread2, thread3, thread4, thread5;

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
    sem_destroy(&sem5);
    sem_post(&sem6);
}

int main(int argc, char const *argv[])
{

    int PORT, i, maxfd, activity;
    printf("Enter Port Number:");
    scanf("%d", &PORT);

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
    printf("Connected to server and ready to process\n");

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
            printf("Message to be processed:: %s \n", buffer);
            strcpy(input, buffer);

            sem_init(&sem6, 0, 1);
            pthread_t threadDec;
            pthread_create(&threadDec, NULL, serverDecoder, (void *)input);
            pthread_join(threadDec, NULL);
            sem_destroy(&sem6);
            send(helpersockfd, input, strlen(input), 0);
        }
    }
}