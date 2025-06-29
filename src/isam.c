#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "../include/isam.h"


static ISAMFILE isam[MAX_ISAM_FILES];


int isam_open(int fd, char const *filename, size_t rec_len) {
    if (fd < 0 || fd >= MAX_ISAM_FILES)
        return -1;

    FILE *fp = fopen(filename, "r+b");
    if (!fp)
        fp = fopen(filename, "w+b");
    if (!fp) {
        printf("Could not open file '%s'\n", filename);
        return -2;
    }
    isam[fd].fp = fp;
    isam[fd].is_open = true;
    isam[fd].rec_len = rec_len;
    return 0;
}


int isam_close(int fd) {
    if (fd < 0 || fd >= MAX_ISAM_FILES)
        return -1;
    fclose(isam[fd].fp);
    isam[fd].is_open = false;
    return 0;
}

/* return codes:                                      */
/*   0 : no error                                     */
/*  -1 : argument error                               */
/*  -2 : file not opened                              */
/*  -3 : error writing record                         */
int isam_write(int fd, int rec_pos, void const *data, serialize_func serialize) {
    size_t bytes;

    if (fd < 0 || fd >= MAX_ISAM_FILES)
        return -1;

    if (!isam[fd].is_open)
        return -2;
    serialize(data, isam[fd].rec_buf, isam[fd].rec_len);
    fseek(isam[fd].fp, isam[fd].rec_len * rec_pos, SEEK_SET);
    bytes = fwrite(isam[fd].rec_buf, isam[fd].rec_len, 1, isam[fd].fp);
    if (bytes != 1) {
        #if DEBUG
            printf("Error on writing Record %d.\n", rec_pos);
        #endif
        return -3;
    }
    #if DEBUG
        printf("Record %d successfully written with %d bytes\n", rec_pos, isam[fd].rec_len);
    #endif
    return 0;
}


/* return codes:                                      */
/*   0 : no error                                     */
/*  -1 : argument error                               */
/*  -2 : file not opened                              */
/*  -3 : error writing record                         */
int isam_read(int fd, int rec_pos, void *data, deserialize_func deserialize) {
    size_t bytes;

    if (fd < 0 || fd >= MAX_ISAM_FILES)
        return -1;

    if (!isam[fd].is_open)
        return -2;

    fseek(isam[fd].fp, isam[fd].rec_len * rec_pos, SEEK_SET);
    bytes = fread(isam[fd].rec_buf, isam[fd].rec_len, 1, isam[fd].fp);
    if (bytes != 1) {
        #if DEBUG
            printf("Error on reading Record %d.", rec_pos);
        #endif
        return -3;
    }
    deserialize(data, isam[fd].rec_buf, isam[fd].rec_len);
    #if DEBUG
        printf("Record %d successfully read with %d bytes\n", rec_pos, isam[fd].rec_len);
    #endif
    return 0;
}
