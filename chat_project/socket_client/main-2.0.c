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
    // COmeçamos criando o socket, utilizando a função socket()
    int socketFD = createTCPIpv4Socket();
    // O primeiro parâmetro, é o domínio, a família do endereço, como vamos utilizar IPv4 é Adress Family_INET
    // Agora, o tipo, vamos utilizar SOCK_STREAM que é o protocolo TCP
    // O ultimo parâmetro, é o protocolo que vamos usar, deixamos em 0 para puxar o protocolo padrão.

    /* A função socket, cria um ponto final para a comunicação e retorna um file descriptor (FD) que aponta para esse ponto-final*/

    struct sockaddr_in *address = createIpv4Address("127.0.0.1", 2000); // Conectando no localhost, especificamente, caçando um processo rodando na mesma máquina 

    int result = connect(socketFD,address, sizeof(*address)); // A função retorna -1 se der errado

    if (result == 0) {
        printf("Connection was succesful\n");
    } else {
        printf("Could not connect to the server\n");
    }

    char* message; 
    message = "GET \\ HTTP/1.1\r\nHost:google.com\r\n\r\n"; // Mandando para o outro lado
    send(socketFD,message,strlen(message), 0);

    char buffer[1024];
    recv(socketFD, buffer, 1024, 0);

    printf("Mensagem recebida %s: \n", buffer);

    return 0;
}

int createTCPIpv4Socket() {
    return socket(AF_INET, SOCK_STREAM, 0); // A função retorna um número inteiro
}