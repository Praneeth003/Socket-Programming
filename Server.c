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
    int i, clientsocket[10], maxclients = 10, helpersocket;
    ssize_t valread;
    int newsocket, addresslen, temp;
    char buffer[1024] = {0};

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

    // Binding socket to the defined port and address. If unsuccessful, then exit.
    if (bind(serversockfd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("Binding Failed");
        exit(EXIT_FAILURE);
    }
    printf("Binding successful \n");

    // Listening...
    if (listen(serversockfd, 10) < 0)
    {
        perror("Listening Error");
        exit(EXIT_FAILURE);
    }
    printf("Listening successful \n");

    // Using select() and FD_SET() to add the server socket to the set of sockets
    FD_ZERO(&readfds);
    FD_SET(serversockfd, &readfds);
    int flag = 0;

    // Accepting the incoming connections
    while (1)
    {
        int maxfd = serversockfd;
        // Add client sockets to the set
        for (i = 0; i < maxclients; i++)
        {
            if (clientsocket[i] > 0)
            {
                FD_SET(clientsocket[i], &readfds);
            }
            // To get the maximum file descriptor number
            if (clientsocket[i] > maxfd)
            {
                maxfd = clientsocket[i];
            }
        }

        // Waiting for some activity on any one of the sockets in the set
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
            printf("New Socket is accepted \n");

            // The first connection should be the helper node
            if (flag == 0)
            {
                helpersocket = newsocket;
                flag = 1;
            }

            // Else, the connection is a client socket
            else
            {
                // Add this new socket to array of sockets
                for (i = 0; i < maxclients; i++)
                {
                    // Check if the position is empty
                    if (clientsocket[i] == 0)
                    {
                        clientsocket[i] = newsocket;
                        break;
                    }
                }
            }
        }

        // Else, its an incoming message from a client
        for (i = 0; i < maxclients; i++)
        {
            if (FD_ISSET(clientsocket[i], &readfds))
            {
                // Check if it was for closing or else read the incoming message
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
                    // Send this message to the helper node for capitalizing vowels
                    if (send(helpersocket, buffer, strlen(buffer), 0) != strlen(buffer))
                    {
                        perror("Sending Error");
                    }
                    memset(buffer, 0, sizeof(buffer));
                    // Receive the capitalized message from the helper node
                    if (recv(helpersocket, buffer, sizeof(buffer) - 1, 0) < 0)
                    {
                        perror("Receiving Error");
                    }
                    buffer[valread] = '\0';
                }
                temp = i;
            }
        }
        // Send the capitalized message to all the clients except the one who sent it
        for (i = 0; i < maxclients; i++)
        {
            if (clientsocket[i] != 0 && i != temp)
            {
                if (send(clientsocket[i], buffer, strlen(buffer), 0) != strlen(buffer))
                {
                    perror("Sending Error");
                }
                printf("%s", buffer);
            }
        }
    }
    close(serversockfd);

    return 0;
}
