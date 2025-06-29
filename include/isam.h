#pragma once

#define STRLEN 30

#define ISAM_WRITE(type, file_id, rec_num, ptr)              \
    do {                                                     \
        __uint8_t __buf__[sizeof(type)];                     \
        serialize_##type((const type *)(ptr), __buf__);      \
        isam_write(file_id, rec_num, __buf__, sizeof(type)); \
    } while(0);

typedef unsigned char UBYTE;

typedef struct {
    FILE    *fp;
    size_t  rec_len;
    bool    is_open;
} ISAMFILE;

// function declarations
int isam_open(int fd, char const *filename, size_t len);
int isam_close(int fd);
int isam_write(int fd, int rec_pos, void const *data, size_t len);
int isam_read(int fd, int rec_pos, void *data, size_t len);