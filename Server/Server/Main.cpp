#include <iostream>

#define _WIN32_WINNT 0x501

#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT "10000"

using std::cout;

int main()
{
	WSADATA wsaData;
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0)
	{
		perror("WSAStartup error");
		exit(2);
	}

	if (system("ipconfig | findstr IPv4 | findstr 192.168") != 0)
	{
		cout << "You don't have public connection\n";
		system("pause");
		exit(3);
	}
	else
		cout << "Enter this IP into your mobile app\n\n";

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
		perror("getaddrinfo error");
		WSACleanup();
		exit(3);
	}

	int listen_socket = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);

	if (listen_socket == INVALID_SOCKET)
	{
		perror("Socket error");
		freeaddrinfo(addr);
		WSACleanup();
		exit(4);
	}

	result = bind(listen_socket, addr->ai_addr, addr->ai_addrlen);

	if (result == SOCKET_ERROR)
	{
		perror("Bind Faild");
		freeaddrinfo(addr);
		closesocket(listen_socket);
		WSACleanup();
		exit(5);
	}

	if (listen(listen_socket, SOMAXCONN) == SOCKET_ERROR)
	{
		perror("Listen socket error");
		closesocket(listen_socket);
		WSACleanup();
		exit(6);
	}

	while (1)
	{
		int client_socket = accept(listen_socket, NULL, NULL);

		if (client_socket == INVALID_SOCKET)
		{
			perror("Accept failed");
			closesocket(listen_socket);
			WSACleanup();
			exit(7);
		}

		const int max_client_buffer_size = 1024;
		char buf[max_client_buffer_size];

		result = recv(client_socket, buf, max_client_buffer_size, 0);

		if (result == SOCKET_ERROR)
		{
			perror("Recv failed");
			closesocket(client_socket);
		}
		else if (result == 0)
		{
			perror("Connection closed...");
		}
		else if (result > 0)
		{
			if (buf[0] == 0)
			{
				cout << "Device connected\n";
			}
			else if (buf[0] == 1)
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