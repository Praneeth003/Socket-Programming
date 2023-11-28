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

#define PORT 3006

int main(int argc, char const *argv[])
{
    int i, clientsocket[10], maxclients = 10;
    ssize_t valread;
    int newsocket, addresslen;
    char buffer[1024] = {0};
    char *message = "Hi Client, this is Server";

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
    printf("Debugging: Binding successful\n");

    // Listening...
    if (listen(serversockfd, 3) < 0)
    {
        perror("Listening Error");
        exit(EXIT_FAILURE);
    }
    printf(" Debugging: Listening successful\n");

    // Accepting the incoming connections
    while (1)
    {
        FD_ZERO(&readfds);
        // Add server socket to the set
        FD_SET(serversockfd, &readfds);
        int maxfd = serversockfd;

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
            printf("select error");
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

            // inform user of socket number - used in send and receive commands
            printf("New connection , socket fd is %d , ip is : %s , port : %d \n", newsocket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));

            // send new connection greeting message

            if (send(newsocket, message, strlen(message), 0) != strlen(message))
            {
                perror("Sending Message to the client error");
            }
            puts("Welcome message is sent successfully");

            // Add new socket to array of sockets
            for (i = 0; i < maxclients; i++)
            {
                // Check if the position is empty
                if (clientsocket[i] == 0)
                {
                    clientsocket[i] = newsocket;
                    printf("Adding to list of sockets as %d\n", i);
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
                valread = read(clientsocket[i], buffer, 1024);
                if (valread == 0)
                {
                    // Somebody is disconnected, get his details and print
                    int result = getpeername(clientsocket[i], (struct sockaddr *)&address, (socklen_t *)&addresslen);
                    printf("Host disconnected , ip %s , port %d \n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));
                    close(clientsocket[i]);
                    clientsocket[i] = 0;
                }
                // Echo back the message that came in
                else
                {
                    printf("%s", buffer);
                    // Set the string terminating NULL byte on the end of the data read
                    buffer[valread] = '\0';
                    send(clientsocket[i], buffer, strlen(buffer), 0);
                }
            }
        }
    }
    close(serversockfd);
    return 0;
}
