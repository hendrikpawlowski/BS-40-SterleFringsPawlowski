#define TEMPERATURE_H

#include <stdio.h>
#include <string.h>
#include "./grovepi.h"
#include <stdlib.h>


int print_temperature (int fileDescriptor) {

    float temp = 0.0;
    int port = 8;

    init();
    pinMode(port, INPUT);

    getTemperature(&temp, port);

    char str[1024];
    // printf("temp: %f\n", temp);
    sprintf(str, "temperature=%.2f\n\n", temp);
    // printf("%s", str);
    write(fileDescriptor, str, strlen(str));

    return 0;
}