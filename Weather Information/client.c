#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define SERVER_IP "127.0.0.1"
#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    WSADATA wsa;
    SOCKET client_socket;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    char location[100];

    //  Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Failed to initialize Winsock. Error Code: %d\n", WSAGetLastError());
        return 1;
    }

    // soket oluşturma
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Socket creation failed. Error Code: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // server yapısı
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(PORT);

    // server bağlantısı
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Connection to server failed. Error Code: %d\n", WSAGetLastError());
        closesocket(client_socket);
        WSACleanup();
        return 1;
    }

    printf("Connected to server.\n");

    // Client loop
    while (1) {
        printf("Enter location (or type 'exit' to quit): ");
        fgets(location, sizeof(location), stdin);
        location[strcspn(location, "\n")] = '\0'; 

        if (strcmp(location, "exit") == 0) {
            break; // exit
        }

        // konum bilgisi
        if (send(client_socket, location, strlen(location), 0) < 0) {
            printf("Send failed. Error Code: %d\n", WSAGetLastError());
            break; // hata kodu
        }

        // hava durumu verisini serverdan getirme
        memset(buffer, 0, BUFFER_SIZE);

        int recv_size = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (recv_size == SOCKET_ERROR) {
            int error_code = WSAGetLastError();
            if (error_code == WSAECONNABORTED) {
                printf("Connection aborted by the server. Error Code: %d\n", error_code);
            } else {
                printf("Receive failed. Error Code: %d\n", error_code);
            }
            break; 
        } else {
            buffer[recv_size] = '\0';

            // şehir bulanamdıysa hata kodu
            if (strcmp(buffer, "City not found") == 0) {
                printf("The city you entered was not found. Please try again.\n");
            } else {
                printf("Server response: %s\n", buffer);
            }
        }
    }

    // kapatma ve celanup
    closesocket(client_socket);
    WSACleanup();
    return 0;
}
