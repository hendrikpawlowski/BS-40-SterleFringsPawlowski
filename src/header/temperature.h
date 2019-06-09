#define TEMPERATURE_H

#include <stdio.h>
#include <string.h>
#include "./grovepi.h"
#include <stdlib.h>


int print_temperature(int fileDescriptor) {

    float temp = 0.0;
    int port = 8;

    init();
    pinMode(port, INPUT);

    getTemperature(&temp, port);

    char str[1024];
    sprintf(str, "temperature=%.2f", temp);
    write(fileDescriptor, str, strlen(str));
    write(fileDescriptor, "\n\n", sizeof("\n\n"));

    return 0;
}