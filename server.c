#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

// Estructura para la tabla de traducción de dominios a IPs y viceversa
struct Entry {
    char *domain;
    char *ip;
};

// Tabla de traducción de dominios a IPs y viceversa
struct Entry tabla[] = {
    {"www.avianca.com", "204.74.99.103"},
    {"www.facebook.com", "104.244.42.129"},
    // Agregar más entradas según sea necesario
};

// Tamaño de la tabla
int tabla_size = sizeof(tabla) / sizeof(tabla[0]);

// Dirección IP y puerto del servidor
char *server_ip = "10.161.62.74"; // Cambia a la IP de tu elección
int server_port = 4455;

int main() {
    WSADATA wsaData;
    SOCKET sockfd, newsockfd;
    struct sockaddr_in serv_addr, cli_addr;
    int clilen;
    char buffer[1024];

    // Inicializar Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Error al inicializar Winsock\n");
        return 1;
    }

    // Crear un socket TCP/IP
    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd == INVALID_SOCKET) {
        printf("Error al abrir el socket\n");
        WSACleanup();
        return 1;
    }

    // Configurar la dirección del servidor
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(server_ip);
    serv_addr.sin_port = htons(server_port);

    // Enlazar el socket al puerto
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) {
        printf("Error al enlazar\n");
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }

    // Escuchar conexiones entrantes
    if (listen(sockfd, 5) == SOCKET_ERROR) {
        printf("Error al escuchar\n");
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }

    clilen = sizeof(cli_addr);

    while (1) {
        printf("Esperando conexión...\n");
        // Esperar una conexión
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd == INVALID_SOCKET) {
            printf("Error al aceptar la conexión\n");
            closesocket(sockfd);
            WSACleanup();
            return 1;
        }

        printf("Conexión establecida desde %s:%d\n", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));

        // Recibir solicitud del cliente
        memset(buffer, 0, sizeof(buffer));
        if (recv(newsockfd, buffer, sizeof(buffer), 0) == SOCKET_ERROR) {
            printf("Error al leer del socket\n");
            closesocket(newsockfd);
            closesocket(sockfd);
            WSACleanup();
            return 1;
        }
        printf("Solicitud recibida: %s\n", buffer);

        // Buscar en la tabla y enviar respuesta
        int i;
        char *response = "No se encontró la IP o dominio en la tabla";
        for (i = 0; i < tabla_size; i++) {
            if (strcmp(buffer, tabla[i].domain) == 0) {
                response = tabla[i].ip;
                break;
            } else if (strcmp(buffer, tabla[i].ip) == 0) {
                response = tabla[i].domain;
                break;
            }
        }

        // Enviar respuesta al cliente
        if (send(newsockfd, response, strlen(response), 0) == SOCKET_ERROR) {
            printf("Error al escribir en el socket\n");
            closesocket(newsockfd);
            closesocket(sockfd);
            WSACleanup();
            return 1;
        }

        closesocket(newsockfd);
    }

    closesocket(sockfd);
    WSACleanup();
    return 0;
}