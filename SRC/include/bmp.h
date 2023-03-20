#ifndef BMP_B_
#define BMP_B_

#include "linker.h"
#include "types.h"

#define PRIVATE_PALETE_ID 0xFE

#pragma pack(push, 1)

struct BITMAP_INTRO
{
    WORD bfType;      // specifies the file type
    DWORD bfSize;     // specifies the size in bytes of the bitmap file
    WORD bfReserved1; // reserved; must be 0
    WORD bfReserved2; // reserved; must be 0
    DWORD bfOffBits;  // specifies the offset in bytes from the bitmapfileheader to the bitmap bits
};

struct BITMAP_INFOHEADER
{
    DWORD biSize;
    LONG biWidth;
    LONG biHeight;
    WORD biPlanes;
    WORD biBitCount;
    DWORD biCompression;
    DWORD biSizeImage;
    LONG biXPelsPerMeter;
    LONG biYPelsPerMeter;
    DWORD biClrUsed;
    DWORD biClrImportant;
};

struct RGBA32_ITEM
{
    BYTE b;
    BYTE g;
    BYTE r;
    BYTE a;
};

typedef struct RGBA32_ITEM BITMAP_PALETE_ITEM;

#define BITMAP_BITSPP 8
#define BITMAP_MAX_RESOLUTION (PICSIZE_A4_300_WIDTH * PICSIZE_A4_300_HEIGHT)

#define BITMAP_PALETE_OFFSET (sizeof(struct BITMAP_INTRO) + sizeof(struct BITMAP_INFOHEADER))

#define BITMAP_PALETE_ITEMS_COUNT 0x100
#define BITMAP_PALETE_SIZE (sizeof(BITMAP_PALETE_ITEM) * BITMAP_PALETE_ITEMS_COUNT)

#define BITMAP_DATA_OFFSET (BITMAP_PALETE_OFFSET + BITMAP_PALETE_SIZE)

struct BITMAP_INFO
{
    struct BITMAP_INTRO intro;
    struct BITMAP_INFOHEADER infoheader;
    BITMAP_PALETE_ITEM palete[BITMAP_PALETE_ITEMS_COUNT];
};

struct PICTURE_SIZE
{
    DWORD width;
    DWORD height;
    DWORD bpl;
    DWORD padded_size;
};

#pragma pack(pop)

#define PICSIZE_DPI_150 150
#define PICSIZE_DPI_300 300
#define PICSIZE_A4_150_WIDTH 1240
#define PICSIZE_A4_150_HEIGHT 1754
#define PICSIZE_A4_300_WIDTH 2480
#define PICSIZE_A4_300_HEIGHT 3508

void PSRFUNC get_picture_sizes(WORD resolution, struct PICTURE_SIZE *size);

DWORD PSRFUNC get_row_size(WORD width, BYTE bpp);

void PSRFUNC prepare_bitmap(struct BITMAP_INFO *info, struct PICTURE_SIZE *sizes);
void PSRFUNC put_palete(BYTE *palete, struct BITMAP_INFO *info);

void print_palete(BITMAP_PALETE_ITEM *palete);

void PSRFUNC draw_point(DWORD pos_x, DWORD pos_y, BYTE color_id,
                        DWORD bpl, BYTE *bitmap_data_rows);

extern void *memcpy(void *src, const void *dst, size_t size);

#endif