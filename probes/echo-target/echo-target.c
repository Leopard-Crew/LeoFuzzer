#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    FILE *fp;
    unsigned char buffer[4096];
    size_t bytes_read;
    unsigned long total_bytes;

    if (argc != 2) {
        fprintf(stderr, "LEOFUZZ:ERROR usage: echo-target input-file\n");
        return 2;
    }

    fp = fopen(argv[1], "rb");

    if (fp == 0) {
        fprintf(stderr, "LEOFUZZ:REJECT could-not-open-input\n");
        return 1;
    }

    total_bytes = 0;

    for (;;) {
        bytes_read = fread(buffer, 1, sizeof(buffer), fp);

        if (bytes_read > 0) {
            total_bytes += (unsigned long)bytes_read;
        }

        if (bytes_read < sizeof(buffer)) {
            if (ferror(fp)) {
                fclose(fp);
                fprintf(stderr, "LEOFUZZ:REJECT read-error\n");
                return 1;
            }

            break;
        }
    }

    fclose(fp);

    printf("LEOFUZZ:OK bytes=%lu\n", total_bytes);
    return 0;
}
