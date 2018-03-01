#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>

#define PORT "10000"

using std::cerr;
using std::cout;

int main()
{
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0)
    {
        cerr << "WSAStartup error: " << result << "\n";
        return result;
    }

    char hostname[255];
    struct hostent *he;
    struct in_addr **addr_list;
    gethostname(hostname, 255);
    if ((he = gethostbyname(hostname)) == NULL)
        cerr << "gethostbyname error";
    else
    {
        addr_list = (struct in_addr **)he->h_addr_list;
        for(int i = 0; addr_list[i] != NULL; i++) {
            if (strcmp(inet_ntoa(*addr_list[i]), "192.168") == true)
                cout << "Enter this IP in your mobile app: " << inet_ntoa(*addr_list[i]) << "\n\n";
        }
    }

    struct addrinfo* addr = nullptr;
    struct addrinfo hints;

    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    result = getaddrinfo(NULL, PORT, &hints, &addr);

    if (result != 0)
    {
        cerr << "getaddrinfo error: " << result << "\n";
        WSACleanup();
        return 1;
    }

    int listen_socket = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);

    if (listen_socket == INVALID_SOCKET)
    {
        cerr << "Socket error: " << WSAGetLastError() << "\n";
        freeaddrinfo(addr);
        WSACleanup();
        return 1;
    }

    result = bind(listen_socket, addr->ai_addr, addr->ai_addrlen);

    if (result == SOCKET_ERROR)
    {
        cerr << "Bind Faild: " << WSAGetLastError() << "\n";
        freeaddrinfo(addr);
        closesocket(listen_socket);
        WSACleanup();
        return 1;
    }

    if (listen(listen_socket, SOMAXCONN) == SOCKET_ERROR)
    {
        cerr << "Listen socket error: " << WSAGetLastError();
        closesocket(listen_socket);
        WSACleanup();
        return 1;
    }

    cout << "Awaiting your comand\n\n";

    while (1)
    {
        int client_socket = accept(listen_socket, NULL, NULL);

        if (client_socket == INVALID_SOCKET)
        {
            cerr << "Accept failed: " << WSAGetLastError() << "\n";
            closesocket(listen_socket);
            WSACleanup();
            return 1;
        }

        const int max_client_buffer_size = 1024;
        char buf[max_client_buffer_size];

        result = recv(client_socket, buf, max_client_buffer_size, 0);

        if (result == SOCKET_ERROR)
        {
            cerr << "Recv failed: " << result << "\n";
            closesocket(client_socket);
        }
        else if (result == 0)
        {
            cerr << "Connection closed...\n";
        }
        else if (result > 0)
        {
            if (buf[0] == 1)
            {
                keybd_event(VK_RIGHT, 0x45,
                            KEYEVENTF_EXTENDEDKEY | 0,
                            0);
                cout << "Right button was clicked\n";
            }
            else if (buf[0] == 2)
            {
                keybd_event(VK_LEFT, 0x45,
                            KEYEVENTF_EXTENDEDKEY | 0,
                            0);
                cout << "Left button was clicked\n";
            }
            closesocket(client_socket);
        }
    }

    closesocket(listen_socket);
    freeaddrinfo(addr);
    WSACleanup();

    system("pause");
    return 0;
}
