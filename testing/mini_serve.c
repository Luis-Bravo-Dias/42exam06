#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_CLIENTS 128
#define BUFFER_SIZE 200000

int main(int ac, char **av)
{
    if (ac != 2)
    {
        fprintf(stderr, "Usage: %s <port>\n", av[0]);
        exit(1);
    }

    int clientSockets[MAX_CLIENTS];
    int next_id = 0;

    fd_set activeSockets, readySockets;
    char buffer[BUFFER_SIZE];

    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0)
    {
        perror("Error creating server socket");
        exit(1);
    }
    struct sockaddr_in serverAddress = {0};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    serverAddress.sin_port = htons(atoi(av[1]));

    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
    {
        perror("Error listening on servert socket");
        exit(1);
    }

    FD_ZERO(&activeSockets);
    FD_SET(serverSocket, &activeSockets);
    int maxSocket = serverSocket;

    while (1)
    {
        readySockets = activeSockets;
        if (select(maxSocket + 1, &readySockets, NULL, NULL, NULL) < 0)
        {
            perror("Error in select");
            exit(1);
        }

        for (int sockedId = 0; sockedId <= maxSocket; sockedId++)
        {
            if (FD_ISSET(sockedId, &readySockets))
            {
                if (sockedId == serverSocket)
                {
                    
                }
            }
        }
    }
}