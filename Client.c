#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char const *argv[])
{
    int PORT;
    char message[1024];
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
    printf("Debugging: Connected to server\n");

    ssize_t valread;
    char buffer[1024];

    memset(buffer, 0, sizeof(buffer));
    valread = recv(clientsockfd, buffer, 1023, 0);
    if (valread < 0)
    {
        perror("Receiving Error");
    }
    buffer[valread] = '\0'; // Properly terminate the received message
    printf("Received Message from server: %s \n", buffer);

    while (1)
    {
        printf("Type the message:");
        scanf("%s", message);
        memset(buffer, 0, sizeof(buffer));
        ssize_t bytes_sent = send(clientsockfd, message, strlen(message), 0);
        if (bytes_sent != strlen(message))
        {
            perror("Sending Error");
        }
        printf("\n Debugging: Message sent \n");
    }
    close(clientsockfd);
    return 1;
}
