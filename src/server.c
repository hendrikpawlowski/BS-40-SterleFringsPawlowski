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
//#include "./header/ourstrings.h"
#include "./header/humidity.h"
#include "./header/workWithClient.h"

#define true 1
#define false 0


int main() {

    int sock, fileDescriptor, client_len;;
    struct sockaddr_in serverAddr = setUpServer();
    socklen_t addr_size;

    struct sockaddr_in client;
    client_len = sizeof(client);

    int id, shar_mem = 0;

    id = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0600);
    shmat(id, 0, 0);

    sock = socket(AF_INET, SOCK_STREAM, 0);     // socket wird erstellt
    if (sock == -1) printf("socket failed\n");

    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &(int) {1}, sizeof(int));

    int bind_ = bind(sock, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
    if (bind_ == -1) printf("binding failed\n");

    listen(sock, 1);    // wie viele clients dürfen auf das socket zugreifen

    printf("Server is running\n");

    while (true) {

        fileDescriptor = accept(sock, (struct sockaddr *) &client, &client_len);
        if (fileDescriptor > 0) {
            printf("Client connected\n");
        }

        int pid = fork();   // startet neuen Prozess

        if (pid == 0) {  // wenn der Prozess der Kinderprozess ist, wird der Client bearbeitet

            //int ipAddr = inet_ntoa(client.sin_addr);

            //char str[INET_ADDRSTRLEN];
            //strcat(shar_mem, inet_ntop(AF_INET, client.sin_addr, str, INET_ADDRSTRLEN));
            shar_mem = inet_ntoa(client.sin_addr);
            printf("IP ADRESS: %s\n", (char *) shar_mem);
            //shmdt(shar_mem);
            //shmctl(id, IPC_RMID, 0);

            workWithClient(fileDescriptor);

        } else if (pid > 0) {    // wenn der Prozess der Elternprozess ist, wird der fileDescriptor geschlossen
            close(fileDescriptor);
        } else if (pid < 0) {
            printf("fork failed\n");

            // nur da damit kein endles loop angezeigt wird
            return 0;
        }
    }
    return 0;
}
