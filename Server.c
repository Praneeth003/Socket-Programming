#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 6000

int main(int argc, char const *argv[])
{

    // Socket File descriptor
    int serversockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (serversockfd < 0)
    {
        perror("Socket Failed");
        exit(EXIT_FAILURE);
    }

    // Helps in reusing address and port
    int optval = 1;
    // if (setsockopt(serversockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &optval, sizeof(optval)))
    // {
    //     perror("setsockopt");
    //     exit(EXIT_FAILURE);
    // }

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

    // Binding socket to the defined and port address. In our case address is localhost.
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    socklen_t addresslen = sizeof(address);
    if (bind(serversockfd, (struct sockaddr *)&address, addresslen) < 0)
    {
        perror("Bindig Failed");
        exit(EXIT_FAILURE);
    }
    printf("Binding successful\n");

    // Listening...
    if (listen(serversockfd, 5) < 0)
    {
        perror("Listen Error");
        exit(EXIT_FAILURE);
    }
    printf("Listening...\n");

    // Accepting...
    int newsocket = accept(serversockfd, (struct sockaddr *)&address, &addresslen);
    if (newsocket < 0)
    {
        perror("Accept Error");
        exit(EXIT_FAILURE);
    }
    printf("Accepted a connection\n");

    ssize_t valread;
    char buffer[1024] = {0};
    valread = read(newsocket, buffer, 1023);
    printf("%s \n", buffer);
    char *message = "I am server!";
    send(newsocket, message, strlen(message), 0);
    printf("Message sent");

    close(newsocket);
    close(serversockfd);
    return 0;
}
