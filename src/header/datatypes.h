#include <time.h>

#define GREEN "green"
#define RED "red"
#define clientEntryLength 20
#define serverEntryLength 20
#define ipLength 15
#define statusLength 13
#define functionLength 12
#define timeLength 28
#define msgLength 100


typedef struct s {
    int id;
    char status[statusLength];
    char currentTime[timeLength];
    char ip[ipLength];
    int port;
    char function[functionLength];
    float tempMin;
    float tempCur;
    float tempMax;
} server;

typedef struct c {
    char status[statusLength];
    char currentTime[timeLength];
    char ip[ipLength];
    char function[functionLength];
} client;

typedef struct cPeers {
    client clientList[clientEntryLength];
    server serverList[serverEntryLength];
} peers;

typedef struct message_{
    long mtype;
    char mtext[msgLength];
} message;