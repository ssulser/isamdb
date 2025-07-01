#include <stdio.h>
#include <string.h>

#include "serialize.h"

void serialize_int(int num, unsigned char *buffer) {
    buffer[0] =  num        & 0xFF;
    buffer[1] = (num >>  8) & 0xFF;
    buffer[2] = (num >> 16) & 0xFF;
    buffer[3] = (num >> 24) & 0xFF;
}

void deserialize_int(int *num, unsigned char const *buffer) {
    *num = 0;
    *num  = (buffer[0] | buffer[1] << 8 | buffer[2] << 16 | buffer[3] << 24);
}
