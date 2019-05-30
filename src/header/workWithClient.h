#include "./ourstrings.h"

char buffer[1024];


/*
int disconnect(clientPeers *shar_mem, char *clientip) {
    for (int i = 0; i < entryLength; i++) {
        if (strcmp(shar_mem->clientList[i].ip, clientip) == 0) {
            printf("IF ENTERED\n");
            printf("i: %d\n", i);
            memset(shar_mem->clientList[i].ip, '\0', strlen(shar_mem->clientList[i].ip));
            break;
        }
    }
    return 0;
}*/

int workWithClient(int fileDescriptor, clientPeers *shar_mem, char *clientip) {

    write(fileDescriptor, welcomeString, sizeof(welcomeString));

    while (true) {

        write(fileDescriptor, ">", sizeof(">"));
        // buffer wird geleert
        memset(buffer, '\0', sizeof(buffer));

        int readSize = read(fileDescriptor, buffer, sizeof(buffer));
        if (readSize == 0) {
            createNewEntry(shar_mem, "DISCONNECTED", clientip, "client");
            exit(0);
        }

        /*if (readSize == 0) {
            // es wird ein neuer Eintrag erstellt, in welchem der Client den Status disconneted bekommt
            disconnect(shar_mem, clientip);
            exit(0);
        }*/

        // überflüssige Zeichen wie '\n' werden aus dem buffer entfernt
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

            char output[] = "Following clients are connected\n";
            write(fileDescriptor, output, strlen(output));

            for (int i = 0; i < entryLength; i++) {
                write(fileDescriptor, shar_mem->clientList[i].ip, strlen(shar_mem->clientList[i].ip));
                write(fileDescriptor, "\n", strlen("\n"));
                printf("TEST: %s\n", shar_mem->clientList[i].status);
                write(fileDescriptor, shar_mem->clientList[i].status, strlen(shar_mem->clientList[i].status));
                write(fileDescriptor, "\n", strlen("\n"));
                write(fileDescriptor, shar_mem->clientList[i].function, strlen(shar_mem->clientList[i].function));

                write(fileDescriptor, "\n", strlen("\n"));
                write(fileDescriptor, "\n", strlen("\n"));
            }

        } else if (strcmp("EXIT", buffer) == 0) {

            createNewEntry(shar_mem, "DISCONNECTED", clientip, "client");
            exit(0);

        } else {
            write(fileDescriptor, novalidcommandString, sizeof(novalidcommandString));
        }
    }
}