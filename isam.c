#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#define STRLEN 30

typedef enum { FALSE, TRUE } BOOL;
typedef unsigned char UBYTE;

typedef struct {
    char name[STRLEN];
    long zahl;
    long daten;
} Record;

typedef struct {
    FILE    *fp;
    size_t  rec_len;
    BOOL    is_open;
} ISAMFILE;

static ISAMFILE isam[2];

int isam_open(int fd, char const *filename, size_t len) {
    assert (fd == 1);
    FILE *fp = fopen(filename, "r+b");
    if (!fp)
        fp = fopen(filename, "w+b");
    if (!fp) {
        printf("Could not open file '%s'\n", filename);
        return -1;
    }
    isam[fd].fp = fp;
    isam[fd].is_open = TRUE;
    isam[fd].rec_len = len;
    return 0;
}

int isam_close(int fd) {
    assert (fd == 1);
    fclose(isam[fd].fp);
    isam[fd].is_open = FALSE;
    isam[fd].rec_len = 0;
}

void isam_serialize(Record const *src, UBYTE *buffer, size_t len) {
    int n;
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
    int n;
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

int isam_write(int fd, int rec_pos, void const *data, size_t len) {
    size_t bytes;

    assert (fd == 1);
    assert (isam[fd].is_open);
    assert (isam[fd].rec_len == len);

    rec_pos -= 1;   // start at 0 == 1 for user

    UBYTE *buffer = (UBYTE*) malloc(len);
    isam_serialize(data, buffer, len);
    fseek(isam[fd].fp, len * rec_pos, SEEK_SET);
    bytes = fwrite(buffer, len, 1, isam[fd].fp);
    if (bytes != 1) {
        printf("Error on writing Record %d. Should be %ld bytes, was %ld bytes.\n", rec_pos, len, bytes);
        free(buffer);
        return -1;
    }
    printf("Record %d successfully written with %ld bytes\n", rec_pos, len);
    free(buffer);
    return 0;
}

int isam_read(int fd, int rec_pos, void *data, size_t len) {
    size_t bytes;

    assert (fd == 1);
    assert (isam[fd].is_open);
    assert (isam[fd].rec_len == len);

    rec_pos -= 1; // start at 0 == 1 for user

    UBYTE *buffer = (UBYTE*) malloc(len);
    fseek(isam[fd].fp, len * rec_pos, SEEK_SET);
    bytes = fread(buffer, len, 1, isam[fd].fp);
    if (bytes != 1) {
        printf("Error on reading Record %d.", rec_pos);
        free (buffer);
        return 1;
    }
    isam_deserialize((Record*)data, buffer, len);
    free (buffer);
    return 0;
}

int main(int argc, char const *argv[])
{
    int i;
    int result;

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
    for (i=1; i<=4; i++) {
        result = isam_read(1, i, &daten1, 38);
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
