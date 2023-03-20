#include "argv_help.h"
#include "bmp.h"

#include <stdio.h>

void help()
{
    printf("./* [8-bit bmp source] [binary out]\n");
}

void exit_help(const char *func, const char *message)
{
    fprintf(stderr, "# %10s: %s\n", func, message);
    help();
    exit(1);
}

int main(int argc, char *argv[])
{
    ARGCCHECK(argc, 3);

    struct BITMAP_INFO info;

    char *path_bmp = argv[1],
         *path_output = argv[2];

    FILE *f_bmp = fopen(path_bmp, "rb");
    NULLASSERT(f_bmp, "failed to open bmp");

    FILE *f_output = fopen(path_output, "wb");
    NULLASSERT(f_bmp, "failed to open output");

    fread(&info, sizeof(struct BITMAP_INFO), 1, f_bmp);

    DWORD size_row = get_row_size(info.infoheader.biWidth, BITMAP_BITSPP);
    BYTE *buf_row = calloc(size_row, sizeof(BYTE));
    NULLASSERT(buf_row, "failed to allocate row buffer");

    for (DWORD a = 0; a < info.infoheader.biHeight; ++a)
    {
        fread(buf_row, size_row, sizeof(BYTE), f_bmp);
        for (DWORD b = 0; b < info.infoheader.biWidth; ++b)
        {
            BYTE id_palete_item = buf_row[b];

            DWORD palete_item_mean =
                ((info.palete + id_palete_item)->r +
                 (info.palete + id_palete_item)->g +
                 (info.palete + id_palete_item)->b) /
                3;
            BYTE greyscale = (BYTE)palete_item_mean;
            fwrite(&greyscale, sizeof(BYTE), 1, f_output);
        }
    }

    return EXIT_SUCCESS;
}