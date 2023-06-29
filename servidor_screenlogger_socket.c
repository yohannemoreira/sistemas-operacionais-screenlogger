#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <time.h>  // Inclua a biblioteca time.h

#define MAX_BUFFER_SIZE 1024

void error(const char *message) {
    perror(message);
    exit(1);
}

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddress, clientAddress;
    int serverPort = 8080;
    char *savePath = "/home/yohanne/Documentos/Projeto de SO/Server-Client/imagens";  // Caminho para salvar as imagens recebidas

    // Criando o socket do servidor
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        error("Falha ao criar o socket");
    }

    // Configurando o endereço do servidor
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(serverPort);

    // Vinculando o socket à porta do servidor
    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
        error("Falha ao vincular o socket");
    }

    // Habilitando o servidor para aguardar conexões
    if (listen(serverSocket, 1) < 0) {
        error("Falha ao ouvir conexões");
    }

    printf("Servidor aguardando conexões...\n");

    while (1) {
        socklen_t clientAddressLength = sizeof(clientAddress);

        // Aceitando a conexão do cliente
        clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientAddressLength);
        if (clientSocket < 0) {
            error("Falha ao aceitar a conexão");
        }

        printf("Cliente conectado\n");

        // Recebendo a imagem do cliente
        char buffer[MAX_BUFFER_SIZE];
        ssize_t bytesRead;
        size_t totalBytesReceived = 0;

        FILE *imageFile;
        char imagePath[MAX_BUFFER_SIZE];


        sprintf(imagePath, "print.png", savePath, inet_ntoa(clientAddress.sin_addr), (long)time(NULL));  // Correção do formato de impressão

        imageFile = fopen(imagePath, "wb");
        if (!imageFile) {
            error("Falha ao criar o arquivo de imagem");
        }

        while ((bytesRead = read(clientSocket, buffer, sizeof(buffer))) > 0) {
            fwrite(buffer, 1, bytesRead, imageFile);
            totalBytesReceived += bytesRead;
        }

        fclose(imageFile);
        close(clientSocket);

        printf("Imagem recebida e salva em: %s (%ld bytes)\n", imagePath, totalBytesReceived);
    }

    close(serverSocket);

    return 0;
}
