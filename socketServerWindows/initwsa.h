#define INITWSA_H

int initWSA(){
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        printf("Failed. Error Code : %d", WSAGetLastError());
        return -1;
    }
    return 0;
}