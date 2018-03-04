#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

using std::perror;
using std::cout;

int main()
{
    char buf[1024];
    int sock = 0;
    int listener = 0;
    int bytes_read = 0;
    struct sockaddr_in addr;
    struct ifaddrs *ifAddrsStruct = NULL;
    struct ifaddrs *ifa = NULL;
    void *tmpAddrPtr = NULL;

    getifaddrs(&ifAddrsStruct);

    for (ifa = ifAddrsStruct; ifa != NULL; ifa = ifa->ifa_next)
    {
        if (!ifa->ifa_addr)
            continue;

        if (ifa->ifa_addr->sa_family == AF_INET)
        {
            tmpAddrPtr = &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
            char addressBuffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
            if (strcmp(addressBuffer, "192.168") == true)
                cout << "Enter this IP in your mobile app: " << addressBuffer << "\n\n";
        }
    }

    if (ifAddrsStruct != NULL) freeifaddrs(ifAddrsStruct);

    listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (listener < 0)
    {
        perror("Socket");
        exit(1);
    }

    memset(&addr, 0, sizeof(addr));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(10000);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(listener, (struct sockaddr*)&addr,  sizeof(addr)) < 0)
    {
        perror ("Bind");
        close(listener);
        exit(2);
    }

    listen (listener, 1);
    cout << "Server ready\n\n";

    while (1)
    {
        sock = accept(listener, NULL, NULL);

        if (sock < 0)
        {
            perror("Accept");
            exit(3);
        }

        bytes_read = recv(sock, buf, 1024, 0);

        if (bytes_read < 0)
        {
            perror("Recieve");
            close(sock);
            exit(4);
        }
        else if (bytes_read == 0)
            cout << "Connection closed...\n";
        else
        {
            if (buf[0] == 0)
            {
                cout << "Device connected\n";
            }
            else if (buf[0] == 1)
            {
                cout << "Right was clicked\n";
            }
            else if (buf[0] == 2)
            {
                cout << "Left was clicked\n";
            }
            close(sock);
        }
    }

    return 0;
}
