#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define PORT 3006

int main(int argc, char const *argv[])
{
    // Socket File descriptor
    int clientsockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (clientsockfd < 0)
    {
        perror("Socket Failed");
        return -1;
    }

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &address.sin_addr) <= 0)
    {
        printf("\n Invalid address/ Address not supported \n");
        return -1;
    }

    // Client connection
    socklen_t addresslen = sizeof(address);
    sleep(1); // Add a 1-second delay before the connection attempt

    int status = connect(clientsockfd, (struct sockaddr *)&address, addresslen);
    if (status < 0)
    {
        perror("connect");
        printf("Connection Failed \n");
        return -1;
    }
    printf("Debugging: Connected to server\n");

    ssize_t valread;
    char buffer[1024] = {0};

    char *msg = "Hey, I am client";
    send(clientsockfd, msg, strlen(msg), 0);
    printf("\n Debugging: Message sent \n");
    valread = read(clientsockfd, buffer, 1023);
    printf("%s", buffer);

    close(clientsockfd);
    return 1;
}
