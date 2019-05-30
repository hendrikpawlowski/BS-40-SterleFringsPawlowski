#define entryLength 5
#define ipLength 15
#define statusLength 13
#define functionLength 12


typedef struct c {
    char status[statusLength];
    char ip[ipLength];
    char function[functionLength];
} client;

typedef struct cPeers {
    client clientList[entryLength];
} clientPeers;