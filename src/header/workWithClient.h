#define WORKWITHCLIENT_H

#include "./ourstrings.h"

char buffer[1024];


int workWithClient(int fileDescriptor) {

    write(fileDescriptor, welcomeString, sizeof(welcomeString));

    while (true) {

        write(fileDescriptor, ">", sizeof(">"));
        memset(buffer, '\0', sizeof(buffer));   // buffer wird geleert

        int readSize = read(fileDescriptor, buffer, sizeof(buffer));

        // überflüssige Zeichen wie '\n' werden aus dem buffer entfernt
        clearBuffer(buffer);

        //printf("BUFFER: %s\n", buffer);

        if (strcmp("GET TEMPERATURE", buffer) == 0) {

            int result = print_temperature(fileDescriptor);

        } else if (strcmp("GET HUMIDITY", buffer) == 0) {

            printf("GET HUMIDITY wurde aufgerufen\n");
            int result = print_humidity(fileDescriptor);

        } else if (strcmp("HELP", buffer) == 0) {

            printf("HELP wurde aufgerufen\n");
            write(fileDescriptor, helpString2, sizeof(helpString2));

        } else if (strcmp("PEERS", buffer) == 0) {

            //shmat(id, 0, 0);
            // printf("\n\n\n\nPEERS wurde aufgerufen\n");
            // printf("Clients: %s", (char *) shar_mem);
            /////////////// funktioniert/////////////////////////////////////////////
            // write(fileDescriptor, (char *) shar_mem, strlen((char *) shar_mem));
            // printf("Clients: %s\n", (char *) shar_mem);
            //////////////////////////////////////////////////////////
            //shmdt(shar_mem);
            //shmctl(id, IPC_RMID, 0);

        } else if (strcmp("EXIT", buffer) == 0) {
            exit(0);

        } else {
            write(fileDescriptor, novalidcommandString, sizeof(novalidcommandString));
        }
    }
}