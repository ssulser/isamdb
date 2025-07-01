#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "isam.h"
#include "serialize.h"

#define STRLEN 30

typedef struct {
    char name[STRLEN];
    int zahl;
    int daten;
} Record;


void Record_serialize(void const *src, unsigned char *buffer, int len) {   //int len might not be necessary?
    const Record *rec = (Record*)src;

    memcpy(buffer, rec->name, STRLEN);

    serialize_int(rec->daten, buffer+STRLEN);
    serialize_int(rec->zahl,  buffer+STRLEN+4);
}

void Record_deserialize(void *dest, unsigned char const *buffer, int len) {   //int len might not be necessary?
    Record *rec = (Record *)dest;

    memcpy(rec->name, buffer, STRLEN);

    deserialize_int(&(rec->daten), buffer+STRLEN);
    deserialize_int(&(rec->zahl), buffer+STRLEN+4);
}

int main(int argc, char const *argv[])
{
    int i, result;

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

    if (isam_open(1, "mydata", 38)) {
        exit(1);
    }

    isam_write(1, 0, &daten1, Record_serialize);
    isam_write(1, 1, &daten2, Record_serialize);
    isam_write(1, 2, &daten3, Record_serialize);
    isam_write(1, 3, &daten4, Record_serialize);
    isam_delete(1, 2);

    for (i=0; i<4; i++) {
        result = isam_read(1, i, &daten1, Record_deserialize);
        if (!result) {
            printf("Record %d:\n", i);
            printf("  - name = %30s\n", daten1.name);
            printf("  - daten= %d\n", daten1.daten);
            printf("  - zahl = %d\n", daten1.zahl);
            printf("---------------------\n");
        } else {
            printf("!!! Error on Record %d - Error = %d (%s)\n", i, result, isam_error(result));
        }

    }

    isam_close(1);
    return 0;
}
