#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 8080
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 10


struct City {
    char name[100];
    char weather_info[200];
};

// Hava durumu Verileri
struct City cities[] = {
    {"Adana", "Sunny, 25°C"},
    {"Adiyaman", "Cloudy, 22°C"},
    {"Afyonkarahisar", "Rainy, 18°C"},
    {"Agri", "Windy, 15°C"},
    {"Aksaray", "Clear, 28°C"},
    {"Amasya", "Partly Cloudy, 20°C"},
    {"Ankara", "Sunny, 23°C"},
    {"Antalya", "Clear, 30°C"},
    {"Ardahan", "Foggy, 12°C"},
    {"Artvin", "Cloudy, 18°C"},
    {"Aydin", "Rainy, 17°C"},
    {"Balikesir", "Windy, 15°C"},
    {"Bartin", "Clear, 25°C"},
    {"Batman", "Partly Cloudy, 23°C"},
    {"Bayburt", "Sunny, 28°C"},
    {"Bilecik", "Clear, 22°C"},
    {"Bingol", "Rainy, 19°C"},
    {"Bitlis", "Foggy, 10°C"},
    {"Bolu", "Cloudy, 16°C"},
    {"Burdur", "Clear, 26°C"},
    {"Bursa", "Windy, 17°C"},
    {"Canakkale", "Sunny, 24°C"},
    {"Cankiri", "Clear, 21°C"},
    {"Corum", "Foggy, 13°C"},
    {"Denizli", "Clear, 27°C"},
    {"Diyarbakir", "Sunny, 32°C"},
    {"Duzce", "Rainy, 14°C"},
    {"Edirne", "Windy, 16°C"},
    {"Elazig", "Clear, 30°C"},
    {"Erzincan", "Cloudy, 19°C"},
    {"Erzurum", "Snowy, -5°C"},
    {"Eskisehir", "Clear, 22°C"},
    {"Gaziantep", "Sunny, 33°C"},
    {"Giresun", "Rainy, 18°C"},
    {"Giumushhane", "Clear, 23°C"},
    {"Hakkari", "Partly Cloudy, 20°C"},
    {"Hatay", "Sunny, 28°C"},
    {"Isparta", "Clear, 24°C"},
    {"Istanbul", "Rainy, 20°C"},
    {"Izmir", "Sunny, 27°C"},
    {"Kahramanmaras", "Clear, 28°C"},
    {"Karabuk", "Rainy, 16°C"},
    {"Karaman", "Cloudy, 21°C"},
    {"Kars", "Foggy, 8°C"},
    {"Kastamonu", "Clear, 19°C"},
    {"Kayseri", "Clear, 22°C"},
    {"Kirikkale", "Sunny, 25°C"},
    {"Kirklareli", "Cloudy, 15°C"},
    {"Kocaeli", "Sunny, 21°C"},
    {"Konya", "Clear, 27°C"},
    {"Kutahya", "Cloudy, 18°C"},
    {"Malatya", "Partly Cloudy, 22°C"},
    {"Manisa", "Sunny, 30°C"},
    {"Mardin", "Clear, 33°C"},
    {"Mersin", "Clear, 31°C"},
    {"Mugla", "Sunny, 27°C"},
    {"Mus", "Clear, 23°C"},
    {"Nevsehir", "Clear, 25°C"},
    {"Nigde", "Cloudy, 20°C"},
    {"Ordu", "Sunny, 24°C"},
    {"Osmaniye", "Clear, 28°C"},
    {"Rize", "Rainy, 15°C"},
    {"Sakarya", "Clear, 26°C"},
    {"Samsun", "Cloudy, 19°C"},
    {"Sanliurfa", "Sunny, 35°C"},
    {"Siirt", "Clear, 30°C"},
    {"Sinop", "Rainy, 16°C"},
    {"Sivas", "Clear, 25°C"},
    {"Tekirdag", "Sunny, 23°C"},
    {"Tokat", "Cloudy, 18°C"},
    {"Trabzon", "Rainy, 14°C"},
    {"Tunceli", "Sunny, 22°C"},
    {"Usak", "Clear, 21°C"},
    {"Van", "Sunny, 29°C"},
    {"Yalova", "Partly Cloudy, 25°C"},
    {"Yozgat", "Clear, 22°C"},
    {"Zonguldak", "Rainy, 17°C"}
};


void handle_client(SOCKET client_socket) {
    char buffer[BUFFER_SIZE];
    int recv_size;

    // client iletisim
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        recv_size = recv(client_socket, buffer, BUFFER_SIZE, 0);

        if (recv_size == SOCKET_ERROR) {
            int error_code = WSAGetLastError();
            if (error_code == WSAECONNABORTED) {
                printf("Connection aborted by client. Error Code: %d\n", error_code);
            } else {
                printf("Receive failed. Error Code: %d\n", error_code);
            }
            break; // Exit 
        }

        if (recv_size == 0) {
            printf("Client disconnected.\n");
            break; // client bağlantı cıkısı
        }

        
        buffer[recv_size] = '\0';

        printf("Received location: %s\n", buffer);

        // client çıkmak istiyosa exit
        if (strcmp(buffer, "exit") == 0) {
            printf("Client requested exit. Closing connection.\n");
            break;
        }

        // listede şehri arama
        int city_found = 0;
        for (int i = 0; i < sizeof(cities) / sizeof(cities[0]); i++) {
            if (strcasecmp(cities[i].name, buffer) == 0) {
                city_found = 1;
                // hava durumu verisini clienta gönderme
                if (send(client_socket, cities[i].weather_info, strlen(cities[i].weather_info), 0) == SOCKET_ERROR) {
                    printf("Send failed. Error Code: %d\n", WSAGetLastError());
                    break;
                }
                break;
            }
        }

        if (!city_found) {
            // sehir bulunamadı mesajı
            const char *not_found_msg = "City not found";
            if (send(client_socket, not_found_msg, strlen(not_found_msg), 0) == SOCKET_ERROR) {
                printf("Send failed. Error Code: %d\n", WSAGetLastError());
                break;
            }
        }
    }

    // client bağlantısı biterse
    closesocket(client_socket);
    printf("Client connection closed.\n");
}

int main() {
    WSADATA wsa;
    SOCKET server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    int client_addr_len = sizeof(client_addr);
    HANDLE thread;

   
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Failed to initialize Winsock. Error Code: %d\n", WSAGetLastError());
        return 1;
    }

    // socket oluşturma
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Socket creation failed. Error Code: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // server yapısı oluşturma
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // bind socket
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Bind failed. Error Code: %d\n", WSAGetLastError());
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    // gelen bağlantıyı dinleme
    if (listen(server_socket, MAX_CLIENTS) == SOCKET_ERROR) {
        printf("Listen failed. Error Code: %d\n", WSAGetLastError());
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    printf("Server is listening on port %d...\n", PORT);

    // gelen bağlantı isteğini kabul etme
    while ((client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len)) != INVALID_SOCKET) {
        printf("Client connected: %s\n", inet_ntoa(client_addr.sin_addr));

        // yeni client için thread oluşturma
        thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)handle_client, (LPVOID)client_socket, 0, NULL);
        if (thread == NULL) {
            printf("Failed to create thread. Error Code: %d\n", GetLastError());
            closesocket(client_socket);
        } else {
           
            CloseHandle(thread);
        }
    }

    // kapatma ve cleanup
    if (client_socket == INVALID_SOCKET) {
        printf("Accept failed. Error Code: %d\n", WSAGetLastError());
    }

    closesocket(server_socket);
    WSACleanup();
    return 0;
}
