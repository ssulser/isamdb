#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
    char    key[10];
    int     record_nr;
} Index_Record;


int main(int argc, char const *argv[])
{
    Index_Record idx[5] = {
        { .key = "Sulser", .record_nr = 1 },
        { .key = "Müller", .record_nr = 5 },
        { .key = "Meier", .record_nr = 12 },
        { .key = "Furrer", .record_nr = 4 },
        { .key = "Gerber", .record_nr = 7 }
    };

    FILE *fp = fopen("index.dat", "w+b");
    for (int i=0; i<(sizeof(idx)/sizeof(idx[0])); i++) {
        unsigned char buffer[128];
        strncpy(&buffer[0], idx[i].key, 10);
        buffer[10] = idx[i].record_nr & 0xFF;
        buffer[11] = (idx[i].record_nr >> 8)   & 0xFF;
        buffer[12] = (idx[i].record_nr >> 16)  & 0xFF;
        buffer[13] = (idx[i].record_nr >> 24)  & 0xFF;
        fwrite(&buffer[0], 14, 1, fp);
    }

    fclose(fp);

    return 0;
}
