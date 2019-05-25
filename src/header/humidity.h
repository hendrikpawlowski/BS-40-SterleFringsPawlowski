#define HUMIDITY_H

#include <stdio.h>
#include <string.h>
#include "./grovepi.h"
#include <stdlib.h>


int print_humidity(int fileDescriptor) {

    float humidity = 0.0;
    int port = 8;

    init();
    pinMode(port, INPUT);

    getHumidity(&humidity, port);

    char str[1024];
    // printf("temp: %f\n", temp);
    sprintf(str, "humidity=%.2f\n\n", humidity);
    printf("%s", str);
    write(fileDescriptor, str, strlen(str));

    return 0;
}