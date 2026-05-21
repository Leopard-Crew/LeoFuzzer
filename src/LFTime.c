#include "LFTime.h"

#include <sys/time.h>

double LFNowMilliseconds(void)
{
    struct timeval tv;

    if (gettimeofday(&tv, 0) != 0) {
        return 0.0;
    }

    return ((double)tv.tv_sec * 1000.0) + ((double)tv.tv_usec / 1000.0);
}
