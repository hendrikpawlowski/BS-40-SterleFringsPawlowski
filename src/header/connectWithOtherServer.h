int connectWithOtherServer(char *ip, int port) {

    char buf[1024];
    struct sockaddr_in server;
    int sock;

    sock = socket(AF_INET, SOCK_STREAM, 0);

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(ip);

    connect(sock, (struct sockaddr *) &server, sizeof(server));


    //while (1) {

        char *msg[1024];
        printf("\nEnter message");
        scanf("%s", &msg);

        write(sock, msg, sizeof(msg));

        // send(sock, "GET TEMPERATURE", sizeof("GET TEMPERATURE"), 0);

        // recv(sock, buf, sizeof(buf), 0);
        // memset(buf, '\0', sizeof(buf));

        read(sock, buf, sizeof(buf));
        printf("\nBUFFER\n[%s]\n", buf);

        //return 0;
    //}
}
