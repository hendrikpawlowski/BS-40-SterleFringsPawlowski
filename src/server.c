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
#include "./header/requestOtherServer.h"

#define true 1
#define false 0
#define maxClients 5

// setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &(int) {1}, sizeof(int));


int main() {

    int sock, fileDescriptor, client_len;
    struct sockaddr_in client, serverAddr = setUpServerInfos(7654);
    client_len = sizeof(client);

    int shmid, *shar_mem, clientid;

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
    shmid = shmget(IPC_PRIVATE, sizeof(int[maxClients]), IPC_CREAT | 0600);
    if (shmid == -1) printf("shared memory failed\n");
    shar_mem = shmat(shmid, 0, 0);

    printf("Server is running\n");

    if (fork() == 0) {

        while (true) {

            fileDescriptor = accept(sock, (struct sockaddr *) &client, &client_len);
            if (fileDescriptor == -1) printf("file descriptor failed");

            int pid = fork();

            if (pid == 0) {  // wenn der Prozess der Kinderprozess ist, wird der Client bearbeitet

                clientid = inet_ntoa(client.sin_addr);
                printf("Client %s connected\n", (char *) clientid);

                for (int i = 0; i < maxClients; i++) {
                    if (shar_mem[i] == 0) {
                        shar_mem[i] = clientid;
                        break;
                    }
                }

                workWithClient(fileDescriptor, (int *) shar_mem, clientid);

            } else if (pid > 0) {    // wenn der Prozess der Elternprozess ist, wird der fileDescriptor geschlossen
                close(fileDescriptor);
            } else if (pid < 0) {
                printf("fork failed\n");

                // nur da damit kein endless loop angezeigt wird
                return 0;
            }
        }

    } else {

        ////////////////////////////////////////////////////////////////
        /// Hier kann man Eingaben in der Konsole des Servers machen ///
        ////////////////////////////////////////////////////////////////

        while (true) {
            printf("\nEnter command\nType HELP to see possible commands\n");
            char buffer[1024];

            printf("> ");

            scanf("%s", &buffer);

            char ip[] ="192.168.178.54";
            int port = 4321;

            requestOtherServer(ip, port);

            if (strcmp(buffer, "SHUTDOWN") == 0) {
                return 0;
            }
        }
    }
}
