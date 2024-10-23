#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>

// Função para criar endereço Ipv4
struct sockaddr_in* createIpv4Address(char *ip, int port) {
    struct sockaddr_in *address = malloc(sizeof(struct sockaddr_in));
    address->sin_family = AF_INET;
    address->sin_port = htons(port);

    if(strlen(ip) == 0) {
        address->sin_addr.s_addr = INADDR_ANY;
    } else {
        inet_pton(AF_INET, ip, &address->sin_addr.s_addr);
    } 

    return address;
}

// Estrutura pra armazenar os dados do socket aceito
struct AcceptedSocket {
    int acceptedSocketFD;
    struct sockaddr_in address;
    int error;
    bool acceptedSuccesfully;
};

// Prototipação das funções
int createTCPIpv4Socket();
struct AcceptedSocket* acceptIncomingConnection(int serverSocketFD);
void receiveAndPrintIncomingData(int socketFD);
void startAcceptingIncomingConnections(int serverSocketFD);
void receiveAndPrintIncomingDataOnSeparateThread(struct AcceptedSocket *pSocket);
void* receiveAndPrintIncomingDataWrapper(void *arg);
void sendReceivedMessageToTheOtherClients(char *buffer, int socketFD);

struct AcceptedSocket AcceptedSockets[10];
int acceptedSocketsCount = 0;

// Função principal
int main() {
    int serverSocketFD = createTCPIpv4Socket();
    struct sockaddr_in *serverAddress = createIpv4Address("", 2000);

    int result = bind(serverSocketFD, (struct sockaddr *)serverAddress, sizeof(*serverAddress));
    if (result == 0) {
        printf("Socket was bound succesfully\n");
    }

    int listenResult = listen(serverSocketFD, 10);
    if (listenResult == 0) {
        printf("Server is listening\n");
    }

    startAcceptingIncomingConnections(serverSocketFD);


    shutdown(serverSocketFD, SHUT_RDWR);
    
    return 0;
}

// Função que inicia a aceitação de conexões
void startAcceptingIncomingConnections(int serverSocketFD) {
    while(true) {
        struct AcceptedSocket* clientSocket = acceptIncomingConnection(serverSocketFD);
        if (clientSocket->acceptedSuccesfully) {
            receiveAndPrintIncomingDataOnSeparateThread(clientSocket);
            AcceptedSockets[acceptedSocketsCount++] = *clientSocket;
        } else {
            printf("Failed to accept connection: %d\n", clientSocket->error);
        }
    }
}

// Função para aceitar uma conexão
struct AcceptedSocket* acceptIncomingConnection(int serverSocketFD) {
    struct sockaddr_in clientAddress;
    socklen_t clientAddressSize = sizeof(struct sockaddr_in);
    int clientSocketFD = accept(serverSocketFD, (struct sockaddr*)&clientAddress, &clientAddressSize);


    struct AcceptedSocket* acceptedSocket = malloc(sizeof(struct AcceptedSocket));
    acceptedSocket->address = clientAddress;
    acceptedSocket->acceptedSocketFD = clientSocketFD;
    acceptedSocket->acceptedSuccesfully = clientSocketFD >= 0;

    if (!acceptedSocket->acceptedSuccesfully) {
        acceptedSocket->error = clientSocketFD;
    }

    return acceptedSocket;
}

// Função para receber e imprimir os dados recebidos
void receiveAndPrintIncomingData(int socketFD) {
    char buffer[1024];
    while (true) {
        ssize_t amountReceived = recv(socketFD, buffer, sizeof(buffer), 0);
        if (amountReceived > 0) {
            buffer[amountReceived] = '\0';
            printf("%s\n", buffer);

            sendReceivedMessageToTheOtherClients(buffer, socketFD);
        } else if (amountReceived == 0) {
            break; // Conexão fechada
        } else {
            perror("Erro ao receber dados");
            break;
        }
    }
    close(socketFD);
}

void sendReceivedMessageToTheOtherClients(char *buffer, int socketFD) {
// Passando por todos os clients aceitos
    for (int i = 0; i<acceptedSocketsCount ; i++) {
        if(AcceptedSockets[i].acceptedSocketFD != socketFD) { // Se o FD do socket aceito é diferente do socket FD que enviou a mensagem
            // Basicamente, todo client que não enviou a mensagem
            send(AcceptedSockets[i].acceptedSocketFD, buffer, strlen(buffer), 0);
        }
    }


}

// Função wrapper para receber dados em thread separadas
void* receiveAndPrintIncomingDataWrapper(void *arg) {
    int socketFD = *((int*) arg);
    receiveAndPrintIncomingData(socketFD);
    return NULL;
}

// Função para receber e imprimir os dados em uma thread separada
void receiveAndPrintIncomingDataOnSeparateThread(struct AcceptedSocket *pSocket) {
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, receiveAndPrintIncomingDataWrapper, &(pSocket->acceptedSocketFD));
}

int createTCPIpv4Socket() {
    return socket(AF_INET, SOCK_STREAM, 0); // A função retorna um número inteiro
}
