#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_DOMAIN_LENGTH 100
#define MAX_IP_LENGTH 16
#define MAX_RECORDS 100

struct DNS_Record {
    char domain[MAX_DOMAIN_LENGTH];
    char ip[MAX_IP_LENGTH];
};

int ipv4_pton(const char *src, struct in_addr *dst) {
    return inet_pton(AF_INET, src, dst);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        exit(0);
    }

    char *ip = "35.172.181.200";
    int port = atoi(argv[1]);

    struct DNS_Record dns_records[MAX_RECORDS] = {
        {"www.google.com", "8.8.8.8"},
        {"www.gmail.com", "208.65.153.238"}
        // Agrega más registros aquí si es necesario
    };

    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    char buffer[1024];
    socklen_t addr_size;
    int n;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("[-]socket error");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    n = bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (n < 0) {
        perror("[-]bind error");
        exit(1);
    }

    while (1) {
        memset(buffer, 0, sizeof(buffer));
        addr_size = sizeof(client_addr);
        n = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&client_addr, &addr_size);
        if (n < 0) {
            perror("[-]recvfrom error");
            exit(1);
        }

        struct in_addr addr;
        if (ipv4_pton(buffer, &addr) == 1) {
            printf("[+]IP received: %s\n", buffer);
            char* domain_response = NULL;
            for (int i = 0; i < MAX_RECORDS; i++) {
                if (strcmp(buffer, dns_records[i].ip) == 0) {
                    domain_response = dns_records[i].domain;
                    break;
                }
            }

            if (domain_response == NULL) {
                strcpy(buffer, "Domain not found.");
            } else {
                sprintf(buffer, "http://%s", domain_response);
            }
        } else {
            printf("[+]Domain received: %s\n", buffer);
            char* ip_response = NULL;
            for (int i = 0; i < MAX_RECORDS; i++) {
                if (strcmp(buffer, dns_records[i].domain) == 0) {
                    ip_response = dns_records[i].ip;
                    break;
                }
            }

            if (ip_response == NULL) {
                strcpy(buffer, "IP not found.");
            } else {
                sprintf(buffer, "http://%s", ip_response);
            }
        }

        n = sendto(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&client_addr, sizeof(client_addr));
        if (n < 0) {
            perror("[-]sendto error");
            exit(1);
        }
    }

    close(sockfd);

    return 0;
}
