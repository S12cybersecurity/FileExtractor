#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>


using namespace std;

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    int sockfd, newsockfd, portno;

    cout << "\n\tFile Extractor\n";
    cout << "\nProvide the port number\n";
    cout << "\nServer Port: ";
    cin >> portno;

    socklen_t clilen;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in serv_addr, cli_addr;
    int n;

    // Crear un socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    // Establecer la dirección del servidor
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    // Vincular el socket a la dirección del servidor
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR on binding");
        exit(1);
    }

    // Escuchar conexiones entrantes
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);

    // Aceptar una conexión entrante
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0) {
        perror("ERROR on accept");
        exit(1);
    }

    // Leer los datos del archivo
    FILE *fp = fopen("leak.txt", "wb");
    if (fp == NULL) {
        perror("ERROR opening file");
        exit(1);
    }
    bzero(buffer, BUFFER_SIZE);
    while ((n = recv(newsockfd, buffer, BUFFER_SIZE, 0)) > 0) {
        fwrite(buffer, 1, n, fp);
        bzero(buffer, BUFFER_SIZE);
    }
    fclose(fp);

    close(newsockfd);
    close(sockfd);
    return 0;
}

