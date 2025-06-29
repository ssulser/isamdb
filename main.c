#include <stdio.h>
#include <string.h>

#include "include/isam.h"

typedef struct {
    char name[STRLEN];
    long zahl;
    long daten;
} Record;


void isam_serialize(Record const *src, UBYTE *buffer, size_t len) {
    long num;

    memcpy(buffer, src->name, STRLEN);

    num = src->daten;
    buffer[STRLEN+0] = num & 0xFF;
    buffer[STRLEN+1] = (num >>8 ) & 0xFF;
    buffer[STRLEN+2] = (num >>16) & 0xFF;
    buffer[STRLEN+3] = (num >>24) & 0xFF;

    num = src->zahl;
    buffer[STRLEN+4] = num & 0xFF;
    buffer[STRLEN+5] = (num >>8 ) & 0xFF;
    buffer[STRLEN+6] = (num >>16) & 0xFF;
    buffer[STRLEN+7] = (num >>24) & 0xFF;
}

void isam_deserialize(Record *dest, UBYTE const *buffer, size_t len) {
    long num;

    memcpy(dest->name, buffer, STRLEN);

    num = buffer[STRLEN+0];
    num = num + (buffer[STRLEN+1] << 8);
    num = num + (buffer[STRLEN+2] << 16);
    num = num + (buffer[STRLEN+3] << 24);
    dest->daten = num;

    num = buffer[STRLEN+4];
    num = num + (buffer[STRLEN+5] << 8);
    num = num + (buffer[STRLEN+6] << 16);
    num = num + (buffer[STRLEN+7] << 24);
    dest->zahl = num;
}

int main(int argc, char const *argv[])
{
    Record daten1 = {
        .name = "Simon",
        .daten = 0x12345678,
        .zahl  = 0x47114711
    };

    Record daten2 = {
        .name = "Peter",
        .daten = 11223344,
        .zahl  = 77889911
    };

    Record daten3 = {
        .name = "Alexander",
        .daten = 9876543,
        .zahl  = 13131313
    };

    Record daten4 = {
        .name = "Xenia",
        .daten = 99009900,
        .zahl  = 00110011
    };

    if (isam_open(1, "test.dat", 38)) {
        exit(1);
    }

    isam_write(1, 1, &daten1, 38);
    isam_write(1, 2, &daten2, 38);
    isam_write(1, 3, &daten3, 38);
    isam_write(1, 4, &daten4, 38);

    isam_close(1);

    isam_open(1,"test.dat", 38);
    for (int i=1; i<=4; i++) {
        int result = isam_read(1, i, &daten1, 38);
        if (!result) {
            printf("Record %d:\n", i);
            printf("  - name = %30s\n", daten1.name);
            printf("  - daten= %ld\n", daten1.daten);
            printf("  - zahl = %ld\n", daten1.zahl);
            printf("---------------------\n");
        } else {
            printf("!!! Error on Record %d\n", i+1);
        }

    }

    isam_close(1);
    return 0;
}
