#pragma once

#define MAX_BUFFER_SIZE 256
#define MAX_ISAM_FILES 2

typedef unsigned char ubyte;

typedef enum { false, true } bool;

typedef struct {
    FILE      *fp;                            /* FILE pointer */
    int       rec_len;                        /* length of record */
    ubyte     rec_buf[MAX_BUFFER_SIZE];       /* buffer for record data */
    bool      is_open;                        /* is file open? */
} ISAMFILE;

typedef void (*serialize_func)(void const *src, ubyte *dest, int len);
typedef void (*deserialize_func)(void *dest, ubyte const *src, int len);

/* function declarations */
int isam_write(int fd, int rec_pos, void const *data, serialize_func serialize);
int isam_read(int fd, int rec_pos, void *data, deserialize_func deserialize);
int isam_close(int fd);
int isam_open(int fd, char const *filename, size_t rec_len);
