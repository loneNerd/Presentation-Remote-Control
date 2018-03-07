#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <X11/keysym.h>
#include <X11/extensions/XTest.h>

using std::perror;
using std::cout;
using std::string;

string getIP()
{
    struct ifaddrs *ifAddrsStruct = NULL;
    struct ifaddrs *ifa = NULL;
    void *tmpAddrPtr = NULL;
    string IP = "";

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
                IP = addressBuffer;
        }
    }

    if (ifAddrsStruct != NULL) freeifaddrs(ifAddrsStruct);

    return IP;
}


void  press(bool const &isRight)
{
    Display *display = XOpenDisplay(NULL);
    int keycode = 0;

    if (isRight)
        keycode = XKeysymToKeycode(display, XK_Right);
    else
        keycode = XKeysymToKeycode(display, XK_Left);

    XTestFakeKeyEvent(display, keycode, 1, 0);
    XTestFakeKeyEvent(display, keycode, 0, 0);

    XFlush(display);
    XCloseDisplay(display);
}

int main()
{
    char buf[1024];
    int sock = 0;
    int listener = 0;
    int bytes_read = 0;
    struct sockaddr_in addr;

    if (getIP() == "")
    {
        cout << "You don't have public connection";
        exit(1);
    }
    else
        cout << "Enter this IP in your mobile app: " << getIP() << "\n\n";


    listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (listener < 0)
    {
        perror("Socket");
        exit(2);
    }

    memset(&addr, 0, sizeof(addr));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(10000);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(listener, (struct sockaddr*)&addr,  sizeof(addr)) < 0)
    {
        perror ("Bind");
        close(listener);
        exit(3);
    }

    listen (listener, 1);
    cout << "Server ready\n\n";

    while (1)
    {
        sock = accept(listener, NULL, NULL);

        if (sock < 0)
        {
            perror("Accept");
            exit(4);
        }

        bytes_read = recv(sock, buf, 1024, 0);

        if (bytes_read < 0)
        {
            perror("Recieve");
            close(sock);
            exit(5);
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
                press(true);
                cout << "Right was clicked\n";
            }
            else if (buf[0] == 2)
            {
                press(false);
                cout << "Left was clicked\n";
            }
            close(sock);
        }
    }

    return 0;
}
