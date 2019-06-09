//////////// Unsere Datentypen ////////
#include "header/datatypes.h"
#include "./header/ourstrings.h"
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
#include <sys/sem.h>
#include <sys/msg.h>
#include "./header/grovepi.h"
#include "./header/display.h"
#include "header/queueFunctions.h"
#include "header/semaphoreFunctions.h"
#include "header/helpFunctions.h"
#include "./header/temperature.h"
#include "./header/humidity.h"
#include "./header/workWithClient.h"
#include "./header/connectWithOtherServer.h"


peers *initSharedMemory() {
    int shmid = shmget(IPC_PRIVATE, sizeof(peers), IPC_CREAT | 0600);
    if (shmid == -1) {
        printf("shared memory failed\n");
    }

    peers *shar_mem;
    shar_mem = (peers *) shmat(shmid, 0, 0);
    setupSharMemIDs(shar_mem);

    return shar_mem;
}

int initSocket(struct sockaddr_in serverAddr) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        printf("socket failed\n");
    }
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &(int) {1}, sizeof(int));
    // binde adresse an das socket
    int b = bind(sock, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
    if (b == -1) {
        printf("binding failed\n");
    }
    // wie viele clients dürfen auf das socket zugreifen
    listen(sock, 5);

    return sock;
}

int initSemaphore() {
    int semaphore = semget(IPC_PRIVATE, 1, IPC_CREAT | 0777);
    if (semaphore < 0) {
        printf("semaphore failed");
    }
    return semaphore;
}

int main() {

    int serverPort;
    printf("On which port should your server run?\n");
    scanf("%d", &serverPort);
    int fileDescriptor, client_len;
    struct sockaddr_in clientAddr, serverAddr = setUpServerInfos(serverPort);
    client_len = sizeof(clientAddr);


    peers *shar_mem = initSharedMemory();
    int sock = initSocket(serverAddr);
    int semaphore = initSemaphore();
    int msgQueue = msgget(IPC_PRIVATE, IPC_CREAT | 0666);

    printf("Server is running on Port %d\n", serverPort);

    int displayPeersFork = fork();
    if (displayPeersFork > 0) {

        // in dem Elternprozess von displayPeersFork werden die Peers bearbeitet

        int clientServerFork = fork();
        if (clientServerFork > 0) {
            // der Elternprozess von clientServerFork kümmert sich um eingehende Clients

            while (true) {

                fileDescriptor = accept(sock, (struct sockaddr *) &clientAddr, &client_len);
                if (fileDescriptor == -1) printf("file descriptor failed");

                int pid = fork();

                if (pid == 0) {
                    // im Kindprozess wird der Client bearbeitet

                    char *clientip = inet_ntoa(clientAddr.sin_addr);
                    printf("Client %s connected\n", (char *) clientip);

                    workWithClient(fileDescriptor, shar_mem, semaphore, msgQueue, clientip);

                } else if (pid > 0) {
                    // im Elternprozess wird der fileDescriptor geschlossen
                    // und es wird auf weitere eingehende Clients gewartet
                    close(fileDescriptor);

                } else if (pid < 0) {
                    printf("fork failed\n");
                }
            }
        } else if (clientServerFork == 0) {
            // der Kindprozess von clientServerFork ist dazu da, sich mit anderen Servern zu verbinden
            // Wenn man hier CONNECT aufruft, kann man sich mit einem anderen Server verbinden
            // jeder Server bekommt eine ID zugewiesen
            int id = 1;

            while (1) {
                char input[1024];

                printf("\n>");
                scanf("%s", &input);

                if (strcmp(input, "CONNECT") == 0) {

                    char serverip[ipLength];
                    memset(serverip, 0, sizeof(serverip));
                    int port;
                    printf("IP>");
                    scanf("%s", &serverip);
                    printf("PORT>");
                    scanf("%d", &port);

                    int pid = fork();

                    if (pid > 0) {
                        // Der Elternprozess inkrementiert die id und geht danach wieder an den Anfang der while-Schleife
                        // und wartet auf weitere Befehle
                        id++;
                    } else if (pid == 0) {
                        // Der Kindprozess arbeitet mit dem Server
                        connectWithOtherServer(shar_mem, semaphore, msgQueue, id, serverip, port);
                    } else {
                        printf("fork failed");
                    }
                }
            }
        } else if (clientServerFork < 0) {
            printf("clientServerFork failed\n");
        }
    } else if (displayPeersFork == 0) {
        // in dem Kindprozess von displayPeersFork werden Nachrichten für das LCD-Display empfangen und bearbeitet
        message msg;

        while (1) {
            memset(msg.mtext, 0, sizeof(msg.mtext));
            // type bei msgrcv ist -2 um Nachrichten mit type=1 zu priorisieren (DISCONNECT)
            if (msgrcv(msgQueue, &msg, msgLength, -2, 0) < 0) printf("receiving message failed");

            showOnLCD(msg.mtext);
            sleep(3);
        }
    }
}