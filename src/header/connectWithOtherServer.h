int connectWithOtherServer(peers *shar_mem, int semid, int msgQueue, int id, char *serverip, int port) {

    char buf[1024];
    char msg[] = "GET TEMPERATURE\0";
    struct sockaddr_in server;
    int sock, initialized = -1;

    sock = socket(AF_INET, SOCK_STREAM, 0);

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(serverip);

    // es wird eine Verbindung zu dem Server mit der übergebenen IP und Port aufgebaut
    if (connect(sock, (struct sockaddr *) &server, sizeof(server)) < 0) {
        printf("\n\n(Connection to\nIP: %s\nPort: %d\nfailed)\n\n>", serverip, port);
        exit(0);
    } else {
        printf("\n\n(Connection established)\n\n>");
    }

    while (1) {
        // GET TEMPERATURE wird gesendet
        if (write(sock, msg, sizeof(msg)) == -1) printf("sending failed\n");

        memset(buf, 0, sizeof(buf));
        read(sock, buf, sizeof(buf));

        // Der Buffer wird so oft geleert und ausgelesen bis dieser mit "temperatur=" anfängt
        while (contains(buf, "temperature=", strlen("temperature=")) == -1) {
            memset(buf, 0, sizeof(buf));
            read(sock, buf, sizeof(buf));
        }

        // aus der Antwort einheit=wert wird der wert herausgenommen und als float gespeichert
        char *tempstr;
        // strtok schneidet den token "temperature=" aus dem String heraus
        tempstr = strtok(buf, "temperature=");
        // strtof = string to float
        float temp = strtof(tempstr, NULL);

        if (initialized == -1) {
            // ein neuer Eintrag in der Server Liste wird erstellt
            createNewServerEntry(shar_mem, semid, msgQueue, id, CONNECTED, serverip, port, SERVER, temp);
            initialized++;
        } else {
            // ein Eintrag aus der Server Liste wird mit aktuellen Temperatur Werten aktualisiert
            updateServerEntry(shar_mem, semid, id, temp);
        }
        sleep(10);
    }
}