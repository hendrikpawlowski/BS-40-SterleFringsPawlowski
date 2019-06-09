int send_(int msgQueue, char text[msgLength], long type) {
    message msg;
    msg.mtype = type;
    memset(msg.mtext, 0, sizeof(msg.mtext));
    memcpy(msg.mtext, text, strlen(text));

    if (msgsnd(msgQueue, &msg, strlen(msg.mtext) + 1, 0) < 0) {
        printf("sending failed");
        return -1;
    }

    return 0;
}