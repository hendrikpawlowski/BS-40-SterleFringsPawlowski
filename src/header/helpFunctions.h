
// clearBuffer ist dazu da überflüssige Zeichen wie '\n' oder '\r' aus dem Buffer zu entfernen
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

// setzt alle IDs der Server auf -1
int setupSharMemIDs(peers *shar_mem) {
    for (int i = 0; i < serverEntryLength; i++) {
        shar_mem->serverList[i].id = -1;
    }
    return 0;
}

// erzeugt einen neuen Client Eintrag in dem Shared Memory
int createNewClientEntry(peers *shar_mem, int semaphore, int msgQueue, char *status, char *clientip, char *function) {

    time_t now = time(NULL);
    char *time_str = ctime(&now);
    clearBuffer(time_str);

    down(semaphore);
    /// ANFANG kritischer Bereich
    for (int j = 0; j < clientEntryLength; j++) {
        if (strlen(shar_mem->clientList[j].ip) == 0) {
            memcpy(shar_mem->clientList[j].ip, clientip, strlen(clientip));
            memcpy(shar_mem->clientList[j].currentTime, time_str, strlen(time_str));
            memcpy(shar_mem->clientList[j].status, status, strlen(status));
            memcpy(shar_mem->clientList[j].function, function, strlen(function));
            break;
        }
    }
    /// ENDE kritischer Bereich
    up(semaphore);

    char msg[msgLength];
    long type;
    memset(msg, 0, sizeof(msg));
    // Nachrichten mit dem Status DISCONNECTED haben eine höhere Prioriät
    if (strcmp(status, "DISCONNECTED") == 0) type = 1;
    else type = 1;
    sprintf(msg, "%s\n%s", clientip, status);
    send_(msgQueue, msg, type);

    return 0;
}

// erzeugt einen neuen Server Eintrag in dem Shared Memory
int createNewServerEntry(peers *shar_mem, int semaphore, int msgQueue, int id, char *status, char *ip, int port,
                         char *function,
                         float temp) {

    // Die aktuelle Zeit wird ermittelt
    time_t now = time(NULL);
    char *time_str = ctime(&now);
    clearBuffer(time_str);

    down(semaphore);
    /// ANFANG kritischer Bereich
    for (int i = 0; i < serverEntryLength; i++) {
        if (shar_mem->serverList[i].id == -1) {
            shar_mem->serverList[i].id = id;
            memcpy(shar_mem->serverList[i].ip, ip, strlen(ip));
            shar_mem->serverList[i].port = port;
            memcpy(shar_mem->serverList[i].currentTime, time_str, strlen(time_str));
            memcpy(shar_mem->serverList[i].status, status, strlen(status));
            memcpy(shar_mem->serverList[i].function, function, strlen(function));
            shar_mem->serverList[i].tempMin = temp;
            shar_mem->serverList[i].tempCur = temp;
            shar_mem->serverList[i].tempMax = temp;
            break;
        }
    }
    /// ENDE kritischer Bereich
    up(semaphore);

    return 0;
}

// updatet den Eintrag des Servers, da die Temperatur Daten regelmäßig gemessen werden
int updateServerEntry(peers *shar_mem, int semaphore, int id, float temp) {

    down(semaphore);
    /// ANFANG kritischer Bereich
    for (int i = 0; i < serverEntryLength; i++) {
        if (shar_mem->serverList[i].id == id) {
            if (temp < shar_mem->serverList[i].tempMin) {
                shar_mem->serverList[i].tempMin = temp;
            }
            if (temp > shar_mem->serverList[i].tempMax) {
                shar_mem->serverList[i].tempMax = temp;
            }
            shar_mem->serverList[i].tempCur = temp;
            break;
        }
    }
    /// ENDE kritischer Bereich
    up(semaphore);

    return 0;
}

// wenn der Nutzer PEERS aufruft, werden alle verbundenen Clients und Server in einer Tabelle ausegeben
int showAllPeers(int fileDescriptor, int semaphore, peers *shar_mem) {

    // in tabhead sollen die Überschriften der Tabelle gespeichert werden
    char tabhead[1024];
    memset(tabhead, 0, sizeof(tabhead));
    sprintf(tabhead, "%-15s%-30s%-20s%-20s%-7s%-20s\n", "STATUS", "TIME", "IPv4", "FUNCTION", "PORT",
            "TEMP MIN/CUR/MAX");
    write(fileDescriptor, tabhead, sizeof(tabhead));

    down(semaphore);
    /// ANFANG kritischer Bereich
    for (int i = 0; i < clientEntryLength; i++) {

        if (shar_mem->clientList[i].ip[0] != '\0') {

            char output[1024];
            memset(output, 0, sizeof(output));
            // Die Eintäge der Tabelle werden aufgefüllt und im fileDescriptor ausgegeben
            sprintf(output, "%-15s%-30s%-20s%-20s%-7s%-20s", shar_mem->clientList[i].status,
                    shar_mem->clientList[i].currentTime,
                    shar_mem->clientList[i].ip,
                    shar_mem->clientList[i].function,
                    "---",
                    "---");
            write(fileDescriptor, output, sizeof(output));
            write(fileDescriptor, "\n", sizeof("\n"));
        }
    }

    for (int j = 0; j < serverEntryLength; j++) {
        if (shar_mem->serverList[j].id != -1) {
            char output[1024];
            memset(output, 0, sizeof(output));
            // Die minimale, aktuelle und maximale Temperatur werden zusammen in einem String gespeichert
            char tempString[20];
            memset(tempString, 0, sizeof(tempString));
            sprintf(tempString, "%.2f/%.2f/%.2f", shar_mem->serverList[j].tempMin,
                    shar_mem->serverList[j].tempCur,
                    shar_mem->serverList[j].tempMax);
            // Die Eintäge der Tabelle werden aufgefüllt und im fileDescriptor ausgegeben
            sprintf(output, "%-15s%-30s%-20s%-20s%-7d%-20s", shar_mem->serverList[j].status,
                    shar_mem->serverList[j].currentTime,
                    shar_mem->serverList[j].ip,
                    shar_mem->serverList[j].function,
                    shar_mem->serverList[j].port,
                    tempString);
            write(fileDescriptor, output, sizeof(output));
            write(fileDescriptor, "\n", sizeof("\n"));
        }
    }
    /// ENDE kritischer Bereich
    up(semaphore);
    write(fileDescriptor, "\n", sizeof("\n"));

    return 0;
}

// contains ist eine kleine Hilfsmethode um zu schauen, ob der Anfang von einem char-Array ein anderes char-Array enthält
int contains(const char *x, const char *y, int size) {
    for (int i = 0; i < size; i++) {
        if (x[i] != y[i]) {
            return -1;
        }
    }
    return 0;
}