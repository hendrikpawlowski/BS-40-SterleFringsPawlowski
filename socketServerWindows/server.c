#include <stdio.h>
#include <winsock2.h>
#include "initwsa.h"
#include <unistd.h>
#include <string.h>
#include "./helpfunction.h"

// #pragma comment(lib, "wsock32.lib")

// Compile with following command:
// gcc server.c -lws2_32

#define true 1
#define false 0

int main()
{
    // char a[] = "Hi";
    // char b[] = "Wo";

    // strcat(a, b);
    // printf("%s", a);

    initWSA();
    if (initWSA() == -1)
        return 1;

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
        fileDescriptor = accept(sock, NULL, NULL);
        send(fileDescriptor, "Hi\n", 3, 0);

        char word[1];

        while (recv(fileDescriptor, in, 2000, 0) > 0)
        {

            // char lol = in[0];
            // if(lol == "\n") {
            //     printf("ENTER!!!");
            // }

            

            strcat(word, in);
            printf("%s\n", word);

            if(compare(word, "Hi") == 1) {
                printf("JAAAAAAAAAA!!!!");
                send(fileDescriptor, "\nLol", 4, 0);
            }

            // current = in;
            // int size = 1 + sizeof(word);
            // word[size] = strcat((char *) word, current);
            // printf("%s", word);
            // printf("SIZE: %d\n", sizeof(in));
            // printf("%c", in[0]);
        }

        close(fileDescriptor);
        printf("Filedescriptor closed");
    }

    return 0;
}