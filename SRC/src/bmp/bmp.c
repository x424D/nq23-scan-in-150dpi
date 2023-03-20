#include "bmp.h"

DWORD get_row_size(WORD width, BYTE bpp)
{
    return ((bpp * width + 31) / 32) * 4;
}

void prepare_bitmap(struct BITMAP_INFO *info, struct PICTURE_SIZE *sizes)
{
#ifndef SHELL
    info->intro.bfType = 0x4d42;
    info->intro.bfSize = sizes->padded_size + BITMAP_DATA_OFFSET;
    info->intro.bfOffBits = BITMAP_DATA_OFFSET;
    info->infoheader.biSize = 0x28;

    info->infoheader.biWidth = sizes->width;
    info->infoheader.biHeight = sizes->height;
    info->infoheader.biPlanes = 1;
    info->infoheader.biBitCount = 8;

    info->infoheader.biSizeImage = sizes->padded_size;
#else
    info->intro.bfType = __builtin_bswap16(0x4d42);
    info->intro.bfSize = __builtin_bswap32(sizes->padded_size + BITMAP_DATA_OFFSET);
    info->intro.bfOffBits = __builtin_bswap32(BITMAP_DATA_OFFSET);
    info->infoheader.biSize = __builtin_bswap32(0x28);

    info->infoheader.biWidth = __builtin_bswap32(sizes->width);
    info->infoheader.biHeight = __builtin_bswap32(sizes->height);
    info->infoheader.biPlanes = __builtin_bswap16(1);
    info->infoheader.biBitCount = __builtin_bswap16(8);

    info->infoheader.biSizeImage = __builtin_bswap32(sizes->padded_size);

#endif

    info->intro.bfReserved1 = 0;
    info->intro.bfReserved2 = 0;

    info->infoheader.biCompression = 0;

    info->infoheader.biXPelsPerMeter = 0;
    info->infoheader.biYPelsPerMeter = 0;
    info->infoheader.biClrUsed = 0;
    info->infoheader.biClrImportant = 0;
}

void get_picture_sizes(WORD resolution, struct PICTURE_SIZE *size)
{
    size->height = 0;
    size->width = 0;

    if (resolution == 150)
    {
        size->width = PICSIZE_A4_150_WIDTH;
        size->height = PICSIZE_A4_150_HEIGHT;
    }
    else if (resolution == 300)
    {
        size->width = PICSIZE_A4_300_WIDTH;
        size->height = PICSIZE_A4_300_HEIGHT;
    }

    size->bpl = get_row_size(size->width, BITMAP_BITSPP);
    size->padded_size = size->bpl * size->height;
}

void put_palete(BYTE *palete, struct BITMAP_INFO *info)
{
    memcpy(palete, info->palete, BITMAP_PALETE_SIZE);
}

void draw_point(DWORD pos_x, DWORD pos_y, BYTE color_id,
                DWORD bpl, BYTE *bitmap_data_rows)
{
    DWORD offset = pos_y * bpl + pos_x;
    BYTE *pos = bitmap_data_rows + offset;

    pos[0] = color_id;
    // pos[1] = color_id;
    // pos[bpl] = color_id;
    // pos[bpl + 1] = color_id;
}
