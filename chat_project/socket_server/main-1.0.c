#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <malloc.h>

// Lado do cliente (Sistema Operacional Linux/Mac)

struct sockaddr_in* createIpv4Address(char *ip, int port);

int createTCPIpv4Socket(); // Function Prototype

struct sockaddr_in* createIpv4Address(char *ip, int port) { // Necessário colocar * para alocar um espaço na memória para o retorno da função
    // Um ip é uma string, delimitada por pontos, nós temos que extrair esses números e transformar em inteiro (inet_pton)

    struct sockaddr_in *address = malloc(sizeof(struct sockaddr_in)); // Criando um pointer e alocando memória, acho que são 16 bytes, nesse caso
    address->sin_family = AF_INET; // Domínio da porta, novamente IPv4;
    address->sin_port = htons(port); // Porta do cliente, todas as portas abaixo de mil, são geralmente reservadas.
    
    if(strlen(ip) == 0) { // Se o tamnho da string IP for igual a zero
        address->sin_addr.s_addr = INADDR_ANY; // Se o IP for zero, nós vamos ouvir de qualquer endereço de IP
    } else {
        inet_pton(AF_INET, ip, &address->sin_addr.s_addr); // Os parâmetros são: Tipo de endereço, IPV4 (AF_INET), o IP, que deve ser convertido e qual endereço ele deve ser inserido.
        return address;
    }

    // A função connect aceita os dois tipos de endereço generalizado, mas é bom escolher apropriadamente.
    // Sockaddr_in é o ipv4

}

int main() {

    int serverSocketFD = createTCPIpv4Socket();
    struct sockaddr_in *serverAddress = createIpv4Address("", 2000); // Pegar o endereço para ouvir as mensagens que vem dele

    int result = bind(serverSocketFD, serverAddress, sizeof(*serverAddress)); // Vai alocar um espaço na memória para o processo do servidor assim que a aplicação começar e ocupar a porta que foi inserida no endereço ipv4
    if (result == 0) {
        printf("Socket was bound succesfully\n");
    }

    int listenResult = listen(serverSocketFD, 10); // Quantas máquinas/conexões o servidor vai aceitar


    struct sockaddr_in clientAddress;
    int clientAddressSize = sizeof(struct sockaddr_in);
    int clientSocketFD = accept(serverSocketFD, &clientAddress, &clientAddressSize);
    // A função accept, vai criar um FD para cada cliente tentando conectar ao servidor e vai retornar esse FD (File-Descriptor), a partir desse momento, o servidor vai ter acesso ao FD do lado do cliente
    // o segundo parâmetro é o endereço do cliente remoto
    // O terceiro, ele pede o pointer para o tamanho do endereço, não o tamanho exatemente, então a gente tem que fazer uma gambiarra

    // A partir daqui a gente tem acesso ao socket FD do lado do cliente e podemos utilizar isso, para receber e enviar frames

        char buffer[1024];
    recv(clientSocketFD, buffer, 1024, 0);

    printf("Mensagem recebida %s: \n", buffer);

    return 0;

    // No terminal do Linux, se executarmos:
    // lsof -i :2000, ele vai mostrar exatemente o processo executando na porta 2000
    // COMMAND  PID USER   FD   TYPE DEVICE SIZE/OFF NODE NAME
    // main    9423 4444    3u  IPv4  67714      0t0  TCP *:cisco-sccp (LISTEN)
}


int createTCPIpv4Socket() {
    return socket(AF_INET, SOCK_STREAM, 0); // A função retorna um número inteiro
}