#include <stdio.h>
#include <winsock2.h>
#include "initwsa.h"
#include <unistd.h>
#include <string.h>

// #pragma comment(lib, "wsock32.lib")

// Compile with following command:
// gcc server.c -lws2_32

#define true 1
#define false 0


int main()
{
    initWSA();
    if(initWSA() == -1) return 1;

    SOCKET sock;
    struct sockaddr_in server, client;
    int fileDescriptor, client_len;
    client_len = sizeof(client);
    char in[2000];
    char out[2000];

    sock = socket(AF_INET, SOCK_STREAM, 0);

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(7654);

    bind(sock, (struct sockaddr *)&server, sizeof(server));

    listen(sock, 5);

    printf("Server is running\n");

    while (true)
    {
        fileDescriptor = accept(sock, (struct sockaddr *) &client, &client_len);
        printf("Filedescriptor: %d\n", fileDescriptor);

        printf("%d", read(fileDescriptor, in, 2000));
        
        close(fileDescriptor);
        printf("Filedescriptor closed");
    }

    return 0;
}