#define HELPFUNCTIONS_H

#define true 1
#define false 0


int clearBuffer(char *buffer) {
    for (int i = 0; i < strlen(buffer); i++) {
        if (buffer[i] == '\n') {
            buffer[i] = '\0';
        }
        if (buffer[i] == '\r') {
            buffer[i] = '\0';
        }
    }
    return 0;
}

struct sockaddr_in setUpServerInfos(int port) {
    struct sockaddr_in serverAddr;

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    return serverAddr;
}

