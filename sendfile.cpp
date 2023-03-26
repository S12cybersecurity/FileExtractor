#include <iostream>
#include <string>
#include <winsock2.h>

using namespace std;

#pragma comment(lib, "ws2_32.lib")

int main(){
    string server;
    int port; // Declare "port" as an integer
    string filename;

    cout << "Server: ";
    cin >> server;
    cout << "Port: ";
    cin >> port;
    cout << "Filename: ";
    cin >> filename;


    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "WSAStartup failed with error: " << result << '\n';
        return 1;
    }

    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Failed to create socket: " << WSAGetLastError() << '\n';
        WSACleanup();
        return 1;
    }

    SOCKADDR_IN remoteAddr;
    remoteAddr.sin_family = AF_INET;
    remoteAddr.sin_port = htons(port); // Convert the port from string to integer
    remoteAddr.sin_addr.s_addr = inet_addr(server.c_str()); // server IP address
    result = connect(sock, (SOCKADDR*)&remoteAddr, sizeof(remoteAddr));
    if (result == SOCKET_ERROR) {
        std::cerr << "Failed to connect: " << WSAGetLastError() << '\n';
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    FILE* file = fopen(filename.c_str(), "rb");
    if (!file) {
        std::cerr << "Failed to open file: " << errno << '\n';
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    char buffer[4096];
    while (!feof(file)) {
        size_t bytesRead = fread(buffer, 1, sizeof(buffer), file);
        size_t bytesSent = send(sock, buffer, bytesRead, 0);
        if (bytesSent == SOCKET_ERROR) {
            std::cerr << "Error sending data: " << WSAGetLastError() << '\n';
            fclose(file);
            closesocket(sock);
            WSACleanup();
            return 1;
        }
    }

    fclose(file);
    closesocket(sock);
    WSACleanup();

    return 0;
}
