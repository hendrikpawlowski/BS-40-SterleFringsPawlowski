#define HELPFUNCTIONS_H

#define true 1
#define false 0


int compare(char *x, char *y, int sizex, int sizey) {

    if(sizex != sizey) return false;

    for (int i = 0; i < sizex; i++) {

        // printf("x: %c\n", x[i]);
        // printf("y: %c\n", y[i]);

        if (x[i] != y[i]) {
            // printf("return false\n");
            return false;
        }
    }
    // printf("return true\n");
    return true;

    /*
    for (int i;; i++) {
        if (x[i] != y[i]) {
            printf("return false\n");
            return false;
        }

        if (x[i] == '\0' && y[i] == '\0') {
            printf("return true\n");
            return true;
        }
    }
    */
}


struct sockaddr_in setUpServer() {
    struct sockaddr_in serverAddr;

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(7654);

    return serverAddr;
}