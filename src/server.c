#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include "./header/helpfunctions.h"
#include "./header/grovepi.h"
#include "./header/temperature.h"
#include "./header/ourstrings.h"
#include "./header/humidity.h"

#define true 1
#define false 0

// Escape character is '^]'

int main() {

    int sock;
    char buffer[1024];
    struct sockaddr_in serverAddr = setUpServer();
    socklen_t addr_size;

    struct sockaddr_in client;
    int fileDescriptor, client_len;
    client_len = sizeof(client);

    sock = socket(AF_INET, SOCK_STREAM, 0);     // socket wird erstellt
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &(int) {1}, sizeof(int));
    bind(sock, (struct sockaddr *) &serverAddr, sizeof(serverAddr));    // das socket wird an eine Adresse gebunden
    listen(sock, 1);    // wie viele clients dürfen auf das socket zugreifen

    printf("Server is running\n");

    while (true) {

        // int result = fork();

        fileDescriptor = accept(sock, (struct sockaddr *) &client, &client_len);
        if (fileDescriptor > 0) {
            printf("Client connected\n");
        }

        int pid = fork();   // startet neuen Prozess

        if (pid > 0) {  // wenn der Prozess der Elternprozess ist, wird der Client bearbeitet

            write(fileDescriptor, welcomeString, sizeof(welcomeString));

            while (true) {

                write(fileDescriptor, ">", sizeof(">"));
                memset(buffer, '\0', sizeof(buffer));   // buffer wird geleert

                read(fileDescriptor, buffer, sizeof(buffer));   // es wird gewartet, bis der client etwas eingegeben hat
                printf("BUFFER: %s\n", buffer);

                if (compare(tempString, buffer, sizeof(tempString) - 1, strlen(buffer) - 2)) {

                    printf("GET TEMPERATURE wurde aufgerufen\n");
                    int result = print_temperature(fileDescriptor);

                } else if (compare(humString, buffer, sizeof(humString) - 1, strlen(buffer) - 2)) {

                    printf("GET HUMIDITY wurde aufgerufen\n");
                    int result = print_humidity(fileDescriptor);

                } else if (compare(helpString, buffer, sizeof(helpString) - 1, strlen(buffer) - 2)) {

                    printf("HELP wurde aufgerufen\n");
                    write(fileDescriptor, helpString2, sizeof(helpString2));

                } else {
                    write(fileDescriptor, novalidcommandString, sizeof(novalidcommandString));
                }
            }
        } else {    // wenn der Prozess der Kindprozess ist, wird der fileDescriptor geschlossen
            close(fileDescriptor);
        }
    }
    return 0;
}
