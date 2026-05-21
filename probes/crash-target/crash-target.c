#include <signal.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
    FILE *fp;
    char buffer[16];

    if (argc != 2) {
        fprintf(stderr, "LEOFUZZ:ERROR usage: crash-target input-file\n");
        return 2;
    }

    fp = fopen(argv[1], "rb");

    if (fp == 0) {
        fprintf(stderr, "LEOFUZZ:REJECT could-not-open-input\n");
        return 1;
    }

    memset(buffer, 0, sizeof(buffer));
    fread(buffer, 1, sizeof(buffer) - 1, fp);
    fclose(fp);

    if (strncmp(buffer, "CRASH", 5) == 0) {
        raise(SIGSEGV);
    }

    printf("LEOFUZZ:OK no-crash\n");
    return 0;
}
