#ifndef __SERIALIZE_H__
#define __SERIALIZE_H__

void serialize_int(int num, unsigned char *buffer);
void deserialize_int(int *num, unsigned char const *buffer);

#endif
