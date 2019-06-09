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
#include <sys/sem.h>
#include "./header/mysemaphore.h"
#include "./header/helpfunctions.h"
#include "./header/grovepi.h"
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


    //int lol;
    //init();
    //printf("Lol: %d", lol);


    int serverPort;
    printf("On which port should your server run?\n");
    scanf("%d", &serverPort);
    int fileDescriptor, client_len;
    struct sockaddr_in clientAddr, serverAddr = setUpServerInfos(serverPort);
    client_len = sizeof(clientAddr);

    // shared memory wird zugewiesen
    /*shmid = shmget(IPC_PRIVATE, sizeof(peers), IPC_CREAT | 0600);
    if (shmid == -1) {
        printf("shared memory failed\n");
        return -1;
    }
    peers *shar_mem;
    shar_mem = (peers *) shmat(shmid, 0, 0);
    setupSharMem(shar_mem);
*/


    peers *shar_mem = initSharedMemory();


    /*
    // socket wird erstellt
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        printf("socket failed\n");
        return -1;
    }
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &(int) {1}, sizeof(int));
    // binde adresse an das socket
    int b = bind(sock, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
    if (b == -1) {
        printf("binding failed\n");
        return -1;
    }

    // wie viele clients dürfen auf das socket zugreifen
    listen(sock, 5);
     */
    int sock = initSocket(serverAddr);

    /*
    // Semaphore wird erstellt
    semid = semget(IPC_PRIVATE, 1, IPC_CREAT | 0777);
    if (semid < 0) {
        printf("semaphore failed");
        return -1;
    }
     */
    int semaphore = initSemaphore();

    printf("Server is running on Port %d\n", serverPort);

    // ein Prozess kümmert sich um eingehende Clients und der andere Prozess kann sich mit Servern verbinden
    int clientServerFork = fork();

    if (clientServerFork > 0) {

        while (true) {

            fileDescriptor = accept(sock, (struct sockaddr *) &clientAddr, &client_len);
            if (fileDescriptor == -1) printf("file descriptor failed");

            int pid = fork();

            if (pid == 0) {
                // wenn der Prozess der Kinderprozess ist, wird der Client bearbeitet

                char *clientip = inet_ntoa(clientAddr.sin_addr);
                printf("Client %s connected\n", (char *) clientip);

                // createNewClientEntry(shar_mem, "CONNECTED", clientip, CLIENT_ONLY);
                workWithClient(fileDescriptor, shar_mem, semaphore, clientip);

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
    } else if (clientServerFork == 0) {

        // Wenn man hier CONNECT aufruft, kann man sich mit einem anderen Server verbinden
        // jeder Server bekommt eine ID zugewiesen
        int id = 1;

        while (true) {
            char input[1024];

            printf("\n>");
            scanf("%s", &input);

            if (strcmp(input, "CONNECT") == 0) {

                char ip[ipLength];
                int port;
                printf("IP>");
                scanf("%s", &ip);
                printf("PORT>");
                scanf("%d", &port);

                int pid = fork();

                if (pid > 0) {
                    // Der Elternprozess inkrementiert die id und geht danach wieder an den Anfang der while-Schleife
                    // und wartet auf weitere Befehle
                    id++;
                } else if (pid == 0) {
                    // Der Kindprozess arbeitet mit dem Server
                    connectWithOtherServer(shar_mem, semaphore, id, ip, port);
                } else {
                    printf("fork failed");
                }
            } else if (strcmp(input, "SHUTDOWN") == 0) {
                printf("Server closed");
                return 0;
            }
        }
    } else if (clientServerFork < 0) {
        printf("first fork failed\n");
    }
}