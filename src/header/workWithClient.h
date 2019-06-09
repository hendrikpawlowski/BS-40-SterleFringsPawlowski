#include "./ourstrings.h"


int workWithClient(int fileDescriptor, peers *shar_mem, int semaphore, char *clientip) {

    createNewClientEntry(shar_mem, semaphore, "CONNECTED", clientip, CLIENT_ONLY);

    char buffer[1024];
    write(fileDescriptor, welcomeString, sizeof(welcomeString));

    while (true) {

        write(fileDescriptor, ">", sizeof(">"));
        // buffer wird geleert
        memset(buffer, '\0', sizeof(buffer));

        int readSize = read(fileDescriptor, buffer, sizeof(buffer));
        if (readSize == 0) {
            createNewClientEntry(shar_mem, semaphore, "DISCONNECTED", clientip, CLIENT_ONLY);
            exit(0);
        }
        printf("BUFFER: %s\n", buffer);

        clearBuffer(buffer);

        if (strcmp("GET TEMPERATURE", buffer) == 0) {

            printf("Client %s called GET TEMPERATURE\n", (char *) clientip);
            print_temperature(fileDescriptor);

        } else if (strcmp("GET HUMIDITY", buffer) == 0) {

            printf("Client %s called GET HUMIDITY\n", (char *) clientip);
            print_humidity(fileDescriptor);

        } else if (strcmp("HELP", buffer) == 0) {

            printf("Client %s called HELP\n", (char *) clientip);
            write(fileDescriptor, helpString2, sizeof(helpString2));

        } else if (strcmp("PEERS", buffer) == 0) {

            printf("Client %s called PEERS\n", (char *) clientip);
            char output[] = "\n\n";
            write(fileDescriptor, output, strlen(output));

            printf("LoL");
            showAllPeers(fileDescriptor, semaphore, shar_mem);

        } else if (strcmp("EXIT", buffer) == 0) {

            printf("Client %s called EXIT\n", (char *) clientip);
            createNewClientEntry(shar_mem, semaphore, "DISCONNECTED", clientip, CLIENT_ONLY);
            exit(0);

        } else {
            write(fileDescriptor, novalidcommandString, sizeof(novalidcommandString));
        }
    }
}