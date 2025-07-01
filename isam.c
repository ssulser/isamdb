#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "isam.h"

#define MAX_FILE_NAME   81
#define MAX_BUFFER_SIZE 256
#define MAX_ISAM_FILES  2
#define RECORD_DELETE   255
#define DATA_EXT        ".dat"
#define IDX_EXT         ".idx"


typedef struct {
    FILE    *fp_dat;                        /* DATA file  */
    FILE    *fp_idx;                        /* INDEX file */
    int     rec_len;                        /* length of record */
    int     last_entry;
    int     deleted_count;                  /* how many deleted records */
    int     valid_entries;                  /* how many valid records */
    bool    is_dirty;                       /* this data not yet written to dat file */
    bool    is_open;                        /* is file open? */
} ISAM_FILE;


static ISAM_FILE isam[MAX_ISAM_FILES];
static __uint8_t rec_buf[MAX_BUFFER_SIZE];


const char * const ISAM_ERROR_STRING[] = {
    "No Error",
    "Cannot open File",
    "File not opened",
    "Wrong file descriptor",
    "Record not read (deleted)",
    "Could not write record to file",
    "Could not read record from file",
};


const char *isam_error(ISAM_ERROR err) {
    return ISAM_ERROR_STRING[err];
}


ISAM_ERROR isam_open(int fd, char const *filename, size_t rec_len) {
    if (fd < 0 || fd >= MAX_ISAM_FILES)
        return ERR_FD_INCORRECT;

    char filename_ext[MAX_FILE_NAME];
    strncpy(filename_ext, filename, MAX_FILE_NAME-1);
    strncat(filename_ext, DATA_EXT, MAX_FILE_NAME-1);

    FILE *fp = fopen(filename_ext, "r+b");
    if (!fp)
        fp = fopen(filename_ext, "w+b");
    if (!fp)
        return ERR_OPEN_FILE;

    isam[fd].fp_dat = fp;

    strncpy(filename_ext, filename, MAX_FILE_NAME-1);
    strncat(filename_ext, IDX_EXT, MAX_FILE_NAME-1);

    fp = fopen(filename_ext, "r+b");
    if (!fp)
        fp = fopen(filename_ext, "w+b");
    if (!fp)
        return ERR_OPEN_FILE;

    isam[fd].fp_idx = fp;
    isam[fd].rec_len = rec_len;
    isam[fd].last_entry = 0;
    isam[fd].deleted_count = 0;
    isam[fd].valid_entries = 0;
    isam[fd].is_dirty = false;
    isam[fd].is_open = true;

    return ERR_NONE;
}


ISAM_ERROR isam_close(int fd) {
    if (fd < 0 || fd >= MAX_ISAM_FILES)
        return ERR_FD_INCORRECT;

    /* TODO: if is_dirty write control block to fp_dat */
    fclose(isam[fd].fp_idx);
    fclose(isam[fd].fp_dat);

    isam[fd].fp_dat = NULL;
    isam[fd].fp_idx = NULL;
    isam[fd].rec_len = 0;
    isam[fd].last_entry = 0;
    isam[fd].deleted_count = 0;
    isam[fd].valid_entries = 0;
    isam[fd].is_dirty = false;
    isam[fd].is_open = false;
    return 0;
}


ISAM_ERROR isam_write(int fd, int rec_pos, void const *data, serialize_func serialize) {
    size_t bytes;

    if (fd < 0 || fd >= MAX_ISAM_FILES)
        return ERR_FD_INCORRECT;

    if (!isam[fd].is_open)
        return ERR_FILE_NOT_OPEN;

    /* write data to data file */
    serialize(data, rec_buf, isam[fd].rec_len);
    fseek(isam[fd].fp_dat, isam[fd].rec_len * rec_pos, SEEK_SET);
    bytes = fwrite(rec_buf, isam[fd].rec_len, 1, isam[fd].fp_dat);
    if (bytes != 1)
        return ERR_WRITE;
    fflush(isam[fd].fp_dat);

    /* write index to index file */
    //fseek(isam[fd].fp_idx);
    return ERR_NONE;
}


ISAM_ERROR isam_read(int fd, int rec_pos, void *data, deserialize_func deserialize) {
    size_t bytes;

    if (fd < 0 || fd >= MAX_ISAM_FILES)
        return ERR_FD_INCORRECT;

    if (!isam[fd].is_open)
        return ERR_FILE_NOT_OPEN;

    fseek(isam[fd].fp_dat, isam[fd].rec_len * rec_pos, SEEK_SET);
    bytes = fread(rec_buf, isam[fd].rec_len, 1, isam[fd].fp_dat);
    if (bytes != 1)
        return ERR_READ;

    if (rec_buf[0] == RECORD_DELETE)
        return ERR_RECORD_DELETED;

    deserialize(data, rec_buf, isam[fd].rec_len);
    return ERR_NONE;
}


ISAM_ERROR isam_delete(int fd, int rec_pos) {
    if (fd < 0 || fd >= MAX_ISAM_FILES)
        return ERR_FD_INCORRECT;

    if (!isam[fd].is_open)
        return ERR_FILE_NOT_OPEN;

    fseek(isam[fd].fp_dat, isam[fd].rec_len * rec_pos, SEEK_SET);
    rec_buf[0] = 255;
    fwrite(rec_buf, 1, 1, isam[fd].fp_dat);
    fflush(isam[fd].fp_dat);
    return ERR_NONE;
}
