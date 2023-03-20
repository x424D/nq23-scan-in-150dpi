#include "types.h"
#include "psr.h"

#ifdef HOST_STATIC
BYTE static_buf_greyscale[BITMAP_MAX_RESOLUTION] = {
    0,
};

void request_greyscale(BYTE *buf_greyscale)
{
    return;
}
#endif