#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char const *argv[])
{
    int PORT, maxfd, activity;
    fd_set readfds;
    printf("Enter Port Number:");
    scanf("%d", &PORT);

    // Socket File descriptor
    int clientsockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (clientsockfd < 0)
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

    // Client connection

    socklen_t addresslen = sizeof(address);
    int status = connect(clientsockfd, (struct sockaddr *)&address, addresslen);
    if (status < 0)
    {
        perror("connect");
        printf("Connection Failed \n");
        return -1;
    }
    printf("Connected to server\n");
    printf("Start chatting!! \n");

    ssize_t valread;
    char buffer[1024];

    while (1)
    {
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds); // Add stdin (user input) to the set
        FD_SET(clientsockfd, &readfds); // Add client socket to the set

        maxfd = (STDIN_FILENO > clientsockfd) ? STDIN_FILENO : clientsockfd;

        activity = select(maxfd + 1, &readfds, NULL, NULL, NULL);
        if (activity < 0)
        {
            perror("Select error");
            exit(EXIT_FAILURE);
        }

        if (FD_ISSET(STDIN_FILENO, &readfds))
        {
            memset(buffer, 0, sizeof(buffer));
            fgets(buffer, sizeof(buffer) - 1, stdin);
            send(clientsockfd, buffer, strlen(buffer), 0);
        }

        if (FD_ISSET(clientsockfd, &readfds))
        {
            memset(buffer, 0, sizeof(buffer));
            valread = recv(clientsockfd, buffer, 1023, 0);
            if (valread < 0)
            {
                perror("Receiving Error");
            }
            else if (valread == 0)
            {
                printf("Server Disconnected\n");
                break;
            }
            else
            {
                buffer[valread] = '\0';
                printf(" Response: %s \n", buffer);
            }
        }
    }
    close(clientsockfd);
    return 1;
}
