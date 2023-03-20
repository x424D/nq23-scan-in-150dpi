#include "bmp.h"

#ifndef PSR
#include <stdio.h>

void print_palete(BITMAP_PALETE_ITEM *palete)
{
    for (DWORD a = 0; a < BITMAP_PALETE_ITEMS_COUNT; ++a)
    {
        printf("[%2X] %2X %2X %2X %2X\n", a,
               palete[a].r, palete[a].g, palete[a].b,
               palete[a].a);
    }
}

#endif