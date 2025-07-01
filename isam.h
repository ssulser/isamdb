#ifndef __ISAM_H__
#define __ISAM_H__

enum ISAM_ERROR_CODE {
    ERR_NONE,
    ERR_OPEN_FILE,
    ERR_FILE_NOT_OPEN,
    ERR_FD_INCORRECT,
    ERR_RECORD_DELETED,
    ERR_WRITE,
    ERR_READ,

};

typedef enum ISAM_ERROR_CODE ISAM_ERROR;
typedef void (*serialize_func)(void const *src, unsigned char *dest, int len);
typedef void (*deserialize_func)(void *dest, unsigned char const *src, int len);

ISAM_ERROR isam_delete(int fd, int rec_pos);

ISAM_ERROR isam_read(int fd, int rec_pos, void *data, deserialize_func deserialize);

ISAM_ERROR isam_write(int fd, int rec_pos, void const *data, serialize_func serialize);

ISAM_ERROR isam_close(int fd);

ISAM_ERROR isam_open(int fd, char const *filename, size_t rec_len);

const char *isam_error(ISAM_ERROR err);

#endif
