//////////// Unsere Datentypen ////////
#include "./header/ourdatatypes.h"
///////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include "./header/helpfunctions.h"
#include "./header/grovepi.h"
#include "./header/temperature.h"
#include "./header/humidity.h"
#include "./header/workWithClient.h"
#include "./header/connectWithOtherServer.h"

#define true 1
#define false 0


int main() {

    int sock, fileDescriptor, client_len;
    struct sockaddr_in clientAddr, serverAddr = setUpServerInfos(7654);
    client_len = sizeof(clientAddr);

    int shmid;

    // socket wird erstellt
    sock = socket(AF_INET, SOCK_STREAM, 0);     // socket wird erstellt
    if (sock == -1) printf("socket failed\n");
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &(int) {1}, sizeof(int));
    // binde adresse an das socket
    int b = bind(sock, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
    if (b == -1) printf("binding failed\n");
    // wie viele clients dürfen auf das socket zugreifen
    listen(sock, 5);

    // ein neues segment wird angelegt
    shmid = shmget(IPC_PRIVATE, sizeof(clientPeers), IPC_CREAT | 0600);
    if (shmid == -1) printf("shared memory failed\n");
    // shar_mem = shmat(shmid, 0, 0);

    printf("Server is running\n");

    int firstFork = fork();

    if (firstFork > 0) {

        while (true) {

            fileDescriptor = accept(sock, (struct sockaddr *) &clientAddr, &client_len);
            if (fileDescriptor == -1) printf("file descriptor failed");

            int pid = fork();

            if (pid == 0) {  // wenn der Prozess der Kinderprozess ist, wird der Client bearbeitet

                clientPeers *shar_mem;
                shar_mem = (clientPeers *) shmat(shmid, 0, 0);

                char *clientip = inet_ntoa(clientAddr.sin_addr);
                printf("Client %s connected\n", (char *) clientip);

                createNewEntry(shar_mem, "CONNECTED", clientip, "client");
                workWithClient(fileDescriptor, shar_mem, clientip);

            } else if (pid > 0) {
                // wenn der Prozess der Elternprozess ist, wird der fileDescriptor geschlossen
                // und der Elternprozess wartet auf weitere eingehende Clients
                close(fileDescriptor);

            } else if (pid < 0) {
                printf("fork failed\n");

                // nur da, damit kein endless loop angezeigt wird
                return 0;
            }
        }
    } else if (firstFork == 0) {

////////////////////////////////////////////////////////////////
/// Hier kann man Eingaben in der Konsole des Servers machen ///
////////////////////////////////////////////////////////////////

        while (true) {
            printf("\nEnter command\nType HELP to see possible commands\n");
            char input[1024];

            printf(">");
            scanf("%s", &input);

            if (strcmp(input, "CONNECT") == 0) {

                char ip[ipLength];
                int port;
                printf("IP: ");
                scanf("%s", &ip);
                printf("\nPORT: ");
                scanf("%d", &port);

                connectWithOtherServer(ip, port);

            } else if (strcmp(input, "SHUTDOWN") == 0) {
                printf("Shutdown ausgeführt");
                return 0;
            }
        }
    } else if (firstFork < 0) {
        printf("first fork failed\n");
    }
}