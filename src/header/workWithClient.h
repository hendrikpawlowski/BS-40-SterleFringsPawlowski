#define WORKWITHCLIENT_H

#include "./ourstrings.h"

#define maxClients 5

char buffer[1024];


int workWithClient(int fileDescriptor, int *shar_mem, int clientid) {

    write(fileDescriptor, welcomeString, sizeof(welcomeString));

    while (true) {

        write(fileDescriptor, ">", sizeof(">"));
        // buffer wird geleert
        memset(buffer, '\0', sizeof(buffer));

        int readSize = read(fileDescriptor, buffer, sizeof(buffer));
        if (readSize == 0) {
            // die ip adresse von dem client wird aus dem shared memory entfernt
            for (int i = 0; i < maxClients; i++) {
                if (shar_mem[i] == clientid) {
                    shar_mem[i] = 0;
                    break;
                }
            }
            exit(0);
        }

        // überflüssige Zeichen wie '\n' werden aus dem buffer entfernt
        clearBuffer(buffer);

        if (strcmp("GET TEMPERATURE", buffer) == 0) {

            printf("Client %s called GET TEMPERATURE\n", (char *) clientid);
            print_temperature(fileDescriptor);

        } else if (strcmp("GET HUMIDITY", buffer) == 0) {

            printf("Client %s called GET HUMIDITY\n", (char *) clientid);
            print_humidity(fileDescriptor);

        } else if (strcmp("HELP", buffer) == 0) {

            printf("Client %s called HELP\n", (char *) clientid);
            write(fileDescriptor, helpString2, sizeof(helpString2));

        } else if (strcmp("PEERS", buffer) == 0) {

            printf("Client %s called PEERS\n", (char *) clientid);

            char output[] = "Following clients are connected\n";
            write(fileDescriptor, output, strlen(output));
            // die ip adresse von dem client wird in das shared memory hineingeschrieben
            for (int i = 0; i < maxClients; i++) {
                if (shar_mem[i] != 0) {
                    write(fileDescriptor, (char *) shar_mem[i], strlen((char *) shar_mem[i]));
                    write(fileDescriptor, "\n", strlen("\n"));
                    printf("Client [%d]: %s\n", i, (char *) shar_mem[i]);
                }
            }
            write(fileDescriptor, "\n", strlen("\n"));

        } else if (strcmp("EXIT", buffer) == 0) {

            // die ip adresse von dem client wird aus dem shared memory entfernt
            for (int i = 0; i < maxClients; i++) {
                if (shar_mem[i] == clientid) {
                    shar_mem[i] = 0;
                    break;
                }
            }
            printf("Client %s disconnected\n", (char *) clientid);
            exit(0);

        } else {
            write(fileDescriptor, novalidcommandString, sizeof(novalidcommandString));
        }
    }
}