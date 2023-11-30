#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <errno.h>
#include <arpa/inet.h>

int main(int argc, char const *argv[])
{
    int PORT;
    printf("Enter Port Number:");
    scanf("%d", &PORT);
    int i, clientsocket[10], maxclients = 10;
    ssize_t valread;
    int newsocket, addresslen, temp;
    char buffer[1024] = {0};
    char *message = "Hi Client, You are connected to the server!! Start chatting!! \n";

    // Set of socket descriptors
    fd_set readfds;

    for (i = 0; i < maxclients; i++)
    {
        clientsocket[i] = 0;
    }

    // Socket File descriptor
    int serversockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (serversockfd < 0)
    {
        perror("Socket Failed");
        exit(EXIT_FAILURE);
    }

    // Helps in reusing address and port
    int optval = 1;
    if (setsockopt(serversockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0)
    {
        perror("setsockopt SO_REUSEADDR");
        exit(EXIT_FAILURE);
    }
    if (setsockopt(serversockfd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval)) < 0)
    {
        perror("setsockopt SO_REUSEPORT");
        exit(EXIT_FAILURE);
    }

    // Type of socket created
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    // socklen_t addresslen = sizeof(address);

    // Binding socket to the defined port and address. In our case, this address is localhost.
    if (bind(serversockfd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("Binding Failed");
        exit(EXIT_FAILURE);
    }
    printf("\n Debugging: Binding successful");

    // Listening...
    if (listen(serversockfd, 10) < 0)
    {
        perror("Listening Error");
        exit(EXIT_FAILURE);
    }
    printf(" \n Debugging: Listening successful");

    // Accepting the incoming connections
    FD_ZERO(&readfds);
    FD_SET(serversockfd, &readfds);
    int maxfd = serversockfd;
    while (1)
    {
        // Add client sockets to the set if they are valid
        for (i = 0; i < maxclients; i++)
        {
            if (clientsocket[i] > 0)
            {
                FD_SET(clientsocket[i], &readfds);
            }
            // Highest File descriptor number
            if (clientsocket[i] > maxfd)
            {
                maxfd = clientsocket[i];
            }
        }

        // Waiting for some activity on one of the sockets
        int activity = select(maxfd + 1, &readfds, NULL, NULL, NULL);
        if (activity < 0 && errno != EINTR)
        {
            printf("\n select error");
        }

        // If something happened on the server socket, then its an incoming connection
        if (FD_ISSET(serversockfd, &readfds))
        {
            // Accepting...
            newsocket = accept(serversockfd, (struct sockaddr *)&address, (socklen_t *)&addresslen);
            if (newsocket < 0)
            {
                perror("Accept Error");
                exit(EXIT_FAILURE);
            }
            printf(" \n Debugging: New Socket is accepted");

            // send new connection greeting message
            if (send(newsocket, message, strlen(message), 0) != strlen(message))
            {
                perror("Sending Message to the client error");
            }

            // Add new socket to array of sockets
            for (i = 0; i < maxclients; i++)
            {
                // Check if the position is empty
                if (clientsocket[i] == 0)
                {
                    clientsocket[i] = newsocket;
                    // printf("Adding to list of sockets as %d\n", i);
                    break;
                }
            }
        }

        // Else, its some IO operation on some other socket
        for (i = 0; i < maxclients; i++)
        {
            if (FD_ISSET(clientsocket[i], &readfds))
            {
                // Check if it was for closing and also read the incoming message
                valread = recv(clientsocket[i], buffer, sizeof(buffer) - 1, 0);
                if (valread < 0)
                {
                    perror("recv error");
                }
                else if (valread == 0)
                {
                    printf("Client disconnected , ip %s , port %d \n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));
                    close(clientsocket[i]);
                    clientsocket[i] = 0;
                }
                else
                {
                    buffer[valread] = '\0';
                }
                temp = i;
            }
        }
        for (i = 0; i < maxclients; i++)
        {
            if (clientsocket[i] != 0 && i != temp)
            {
                if (send(clientsocket[i], buffer, strlen(buffer), 0) != strlen(buffer))
                {
                    perror("Sending Error");
                }
            }
        }
    }
    close(serversockfd);

    return 0;
}
