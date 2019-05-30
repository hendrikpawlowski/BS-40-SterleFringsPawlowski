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

int createNewEntry (clientPeers *shar_mem, char status[statusLength], char *clientip, char function[functionLength]) {
    for (int j = 0; j < entryLength; j++) {
        if (strlen(shar_mem->clientList[j].ip) == 0) {
            memcpy(shar_mem->clientList[j].ip, clientip, strlen(clientip));
            memcpy(shar_mem->clientList[j].status, status, strlen(status));
            memcpy(shar_mem->clientList[j].function, function, strlen(function));
            break;
        }
    }
}