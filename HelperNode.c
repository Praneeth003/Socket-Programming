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

int main(int argc, char const *argv[])
{

    int PORT, maxfd, activity;
    fd_set readfds;
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
        printf("Connection Failed \n");
        return -1;
    }
    printf("Debugging: Connected to server\n");

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
            printf("Received Message from server: %s \n", buffer);
            helper(buffer);
            send(helpersockfd, "OK", 2, 0);
        }
    }
}