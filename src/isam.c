#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

#include "isam.h"

static ISAMFILE isam[2] = {
    { .fp = NULL, .rec_len = 0, .is_open = false }, // fd 0 - perhaps control file
    { .fp = NULL, .rec_len = 0, .is_open = false }  // fd 1
};

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
    isam[fd].is_open = true;
    isam[fd].rec_len = len;
    return 0;
}


int isam_close(int fd) {
    assert (fd == 1);
    fclose(isam[fd].fp);
    isam[fd].is_open = false;
    isam[fd].rec_len = 0;
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
        printf("Error on writing Record %d. Should be %lu bytes, was %lu bytes.\n", rec_pos, len, bytes);
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
    isam_deserialize(data, buffer, len);
    free (buffer);
    return 0;
}
