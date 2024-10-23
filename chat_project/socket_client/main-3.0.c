#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <malloc.h>
#include <stdbool.h>
#include <unistd.h>

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

    /*Esta parte aqui funciona como: escutar o console, mandar todos os caracteres antes do \n ao apertar o enter, para o lado do servidor*/

    char *line = NULL;
    size_t lineSize = 0; // Passando um input padrão no console.
    printf("Escreva e será enviado(type exit...)\n");

    while(true) { // Loop para ficar 'olhando' o tempo todo
        ssize_t charCount = getline(&line, &lineSize, stdin);

        /* Depois de receber esta linha de caracteres, o buffer &line esta cheio de caracteres e o 'charCount'
        está recebendo a quantidade de caracteres dentro do &line*/
        if(charCount > 0) {
            if(strcmp(line, "exit\n")==0) {
                break;
            }
        
        ssize_t amountWasSent = send(socketFD,line,charCount, 0); // Talvez retorne -1 porque ele pode não conseguir mandar nada
        } 
        /* Basicamente, se o charcount for maior do que 0, ele chama a função strcmp, que vai verificar se oque ele enviou
        bate com 'exit', se bater, ele fecha o loop de verificação. */
    }

    close(socketFD);

    return 0;
}

int createTCPIpv4Socket() {
    return socket(AF_INET, SOCK_STREAM, 0); // A função retorna um número inteiro
}