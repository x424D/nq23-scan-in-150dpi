#include <stdio.h>
#include <stdlib.h>

#include "argv_help.h"
#include "psr.h"

BYTE static_buf_greyscale[BITMAP_MAX_RESOLUTION] = {
    0,
};

FILE *f_greyscale = NULL;

void request_greyscale(BYTE *buf_greyscale)
{
    fread(buf_greyscale, BITMAP_MAX_RESOLUTION, sizeof(BYTE), f_greyscale);
}

int main(int argc, char *argv[])
{
    struct PICTURE_SIZE sizes;
    if (argc != 3)
    {
        fprintf(stderr, "./* [DPI:150/300] [greyscale file]\n");
        return EXIT_FAILURE;
    }

    f_greyscale = fopen(argv[2], "rb");
    NULLASSERT(f_greyscale, "failed to open greyscale file");

    WORD resolution = strtoul(argv[1], NULL, 10);
    get_picture_sizes(resolution, &sizes);

    DWORD size_bitmap = BITMAP_DATA_OFFSET + sizes.padded_size;
    BYTE *buf_output = calloc(size_bitmap, sizeof(BYTE));
    NULLASSERT(buf_output, "failed to allocate bitmap buffer");

    entry_psr(resolution, buf_output);

    fwrite(buf_output, size_bitmap, sizeof(BYTE), stdout);

    return EXIT_SUCCESS;
}