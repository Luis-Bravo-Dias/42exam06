#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_CLIENTS 128    // Número máximo de conexões de clientes permitidas
#define BUFFER_SIZE 200000 // Tamanho do buffer usado para troca de mensagens

int main(int argc, char **argv) 
{
    if (argc != 2)  // Verifica se o número de argumentos da linha de comando está incorreto 
    {                         
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);  // Imprime uma mensagem de erro no fluxo de erro padrão
        exit(1);                              // Termina o programa com um código de status não zero
    }

    int clientSockets[MAX_CLIENTS];           // Array para armazenar descritores de socket do cliente
    int next_id = 0;                          // Identificador para a próxima conexão de cliente

    // Conjuntos de descritores de arquivo
    fd_set activeSockets, readySockets;        // Conjuntos de descritores de arquivo para rastrear atividade de socket
    char buffer[BUFFER_SIZE];                  // Buffer para armazenar mensagens recebidas

    // Create the server socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);  // Cria um socket com endereçamento IPv4 e protocolo TCP
    if (serverSocket < 0)  // Check if socket creation failed
    {                    
        perror("Error creating server socket"); // Imprime uma mensagem de erro com uma descrição do erro
        exit(1);                              // Termina o programa com um código de status não zero
    }

    // Set up the server address
    struct sockaddr_in serverAddress = {0};    // Estrutura para armazenar o endereço do servidor
    serverAddress.sin_family = AF_INET;        // Define a família de endereços como IPv4
    serverAddress.sin_addr.s_addr = htonl(INADDR_LOOPBACK);  // Define o endereço IP como localhost
    serverAddress.sin_port = htons(atoi(argv[1]));  // Define o número da porta a partir do argumento da linha de comando

    // Bind the server socket to the specified address
    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) 
    {
        perror("Error binding server socket");  // Imprime uma mensagem de erro se a vinculação falhar
        exit(1);                              // Termina o programa com um código de status não zero
    }

    // Listen for incoming connections
    if (listen(serverSocket, MAX_CLIENTS) < 0) 
    {
        perror("Error listening on server socket");  // Imprime uma mensagem de erro se a escuta falhar
        exit(1);                              // Termina o programa com um código de status não zero
    }

    // Initialise the active sockets set
    FD_ZERO(&activeSockets);                   // Limpa o conjunto de descritores de arquivo ativos
    FD_SET(serverSocket, &activeSockets);      // Adiciona o socket do servidor ao conjunto
    int maxSocket = serverSocket;              // Variável para acompanhar o descritor de socket máximo

    while (1) 
    {
        // Wait for activity on the sockets
        readySockets = activeSockets;           // Copia o conjunto de descritores de arquivo ativos para uso com select()
        if (select(maxSocket + 1, &readySockets, NULL, NULL, NULL) < 0) 
        {
            perror("Error in select");         // Imprime uma mensagem de erro se o select() falhar
            exit(1);                          // Termina o programa com um código de status não zero
        }

        // Check each socket for activity
        for (int socketId = 0; socketId <= maxSocket; socketId++) 
        {
            if (FD_ISSET(socketId, &readySockets)) 
            {
                // New client connection
                if (socketId == serverSocket)  // Verifica se a atividade é no socket do servidor
                {
                    int clientSocket = accept(serverSocket, NULL, NULL);  // Aceita uma nova conexão de cliente
                    if (clientSocket < 0) 
                    {
                        perror("Error accepting client connection");  // Imprime uma mensagem de erro se a aceitação falhar
                        exit(1);                          // Termina o programa com um código de status não zero
                    }

                    // Add the new client socket to the active set
                    FD_SET(clientSocket, &activeSockets);     // Adiciona o socket do cliente ao conjunto de descritores de arquivo ativos
                    maxSocket = (clientSocket > maxSocket) ? clientSocket : maxSocket;  // Atualiza o descritor de socket máximo

                    // Send a welcome message to the client
                    sprintf(buffer, "server: client %d just arrived\n", next_id);  // Prepara a mensagem de boas-vindas
                    send(clientSocket, buffer, strlen(buffer), 0);  // Envia a mensagem de boas-vindas ao cliente

                    // Store the client socket for future reference
                    clientSockets[next_id++] = clientSocket;  // Adiciona o socket do cliente ao array
                } 
                else 
                {
                    // Data received from a client
                    int bytesRead = recv(socketId, buffer, sizeof(buffer) - 1, 0);  // Recebe dados do cliente

                    if (bytesRead <= 0) 
                    {
                        // Client disconnected
                        sprintf(buffer, "server: client %d just left\n", socketId);  // Prepara a mensagem de desconexão

                        // Notify remaining clients about the disconnected client
                        for (int i = 0; i < next_id; i++) 
                        {
                            if (clientSockets[i] != socketId) 
                            {
                                send(clientSockets[i], buffer, strlen(buffer), 0); // Envia a mensagem de desconexão para outros clientes
                            }
                        }

                        // Close the socket and remove it from the active set
                        close(socketId);                          // Fecha o socket do cliente
                        FD_CLR(socketId, &activeSockets);         // Remove o socket do cliente do conjunto de descritores de arquivo ativos
                    } 
                    else 
                    {
                        // Broadcast the received message to all other clients
                        buffer[bytesRead] = '\0';                  // Termina a mensagem recebida com um caractere nulo
                        sprintf(buffer, "client %d: %s\n", socketId, buffer);  // Adiciona o identificador do cliente à mensagem

                        for (int i = 0; i < next_id; i++) 
                        {
                            if (clientSockets[i] != socketId) 
                            {
                                send(clientSockets[i], buffer, strlen(buffer), 0);  // Envia a mensagem para outros clientes
                            }
                        }
                    }
                }
            }
        }
    }
    return 0;
}