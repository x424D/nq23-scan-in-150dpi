#include "psr.h"

// void do_scan(WORD resolution, BYTE *buf_output)
void entry_psr(WORD resolution, BYTE *buf_output)
{
    /*
        1. get buffer for greyscale
        2. request greyscale
        3. put bitmap header (resolution) -> buf_output
        4. place greyscale 'accoding to palete' (resolution) -> buf_output:
            if resolution is 300:
                translate
            if resolution is 150:
                scale (with encoding:))
    */

    BYTE *buf_greyscale = NULL;

    buf_greyscale = get_buf_greyscale();
    request_greyscale(buf_greyscale);

    output_bitmap(resolution, buf_greyscale, buf_output);
}

BYTE *get_buf_greyscale()
{
    return (BYTE *)static_buf_greyscale;
}

void output_bitmap(WORD resolution, BYTE *buf_greyscale, BYTE *buf_output)
{
    struct PICTURE_SIZE sizes;

    struct BITMAP_INFO *bitmap_header = (struct BITMAP_INFO *)buf_output;
    BYTE *bitmap_data_rows = buf_output + BITMAP_DATA_OFFSET;

    get_picture_sizes(resolution, &sizes);
    prepare_bitmap(bitmap_header, &sizes);
    put_palete((BYTE *)static_bitmap_palete, bitmap_header);

    if (resolution == PICSIZE_DPI_300)
    {
        translate_greyscale_to_bmp(&sizes, buf_greyscale, bitmap_data_rows);
    }
    else if (resolution == PICSIZE_DPI_150)
    {
        downgrade_greyscale_to_bmp(&sizes, buf_greyscale, bitmap_data_rows);
    }
}

void translate_greyscale_to_bmp(struct PICTURE_SIZE *sizes, BYTE *buf_greyscale, BYTE *bitmap_data_rows)
{
    BYTE *current_row = bitmap_data_rows;
    BYTE *pos_greyscale = buf_greyscale;

    for (DWORD a = 0; a < sizes->height; ++a)
    {
        for (DWORD b = 0; b < sizes->width; ++b)
        {
            current_row[b] = *pos_greyscale;

            ++pos_greyscale;
        }
        current_row += sizes->bpl;
    }
}

/*
    NOTE: handles only 2* sizes!
*/

void PSRFUNC read_point(BYTE *pos_greyscale, BYTE *point_values, BYTE *p_mean)
{
    WORD sum = 0;
    point_values[SECRET_CHECK_A_POS] =
        pos_greyscale[SECRET_POINT_OFFSET_A];
    sum += point_values[SECRET_CHECK_A_POS];

    point_values[SECRET_CHECK_B_POS] =
        pos_greyscale[SECRET_POINT_OFFSET_B];
    sum += point_values[SECRET_CHECK_B_POS];

    point_values[SECRET_CHECK_M_POS] =
        pos_greyscale[SECRET_POINT_OFFSET_M];
    sum += point_values[SECRET_CHECK_M_POS];

    point_values[SECRET_CHECK_S_POS] =
        pos_greyscale[SECRET_POINT_OFFSET_S];
    sum += point_values[SECRET_CHECK_S_POS];

    *p_mean = sum / 4;
}

void downgrade_greyscale_to_bmp(struct PICTURE_SIZE *sizes, BYTE *buf_greyscale, BYTE *bitmap_data_rows)
{
    BYTE *current_row = bitmap_data_rows;
    BYTE *pos_greyscale = buf_greyscale;
    DWORD point_values_holder = 0;
    BYTE secret = 0;

    BYTE point_mean = 0;

    for (DWORD a = 0; a < sizes->height; ++a)
    {
        for (DWORD b = 0; b < sizes->width; ++b, pos_greyscale += 2)
        {
            if (current_row[b] == PRIVATE_PALETE_ID)
            {
#ifdef HOST_LINKED
                // fprintf(stderr, "skip: %u - %u: 0x%x;\n", b, a, secret);
#endif
                continue;
            }

            read_point(pos_greyscale, (BYTE *)&point_values_holder, &point_mean);

            if (point_mean == PRIVATE_PALETE_ID)
            {
                point_mean -= 1;
            }

            if (is_secret_holder((BYTE *)&point_values_holder))
            {
#ifdef HOST_LINKED
                fprintf(stderr, "secret: %u - %u: 0x%x;\n", b, a, secret);
#endif
                secret = ((BYTE *)&point_values_holder)[SECRET_CHECK_S_POS];
                encode(secret, b, a, sizes->bpl, bitmap_data_rows);
            }
            else
            {
                current_row[b] = point_mean;
            }
        }
        current_row += sizes->bpl;
        // skips second row
        pos_greyscale += PICSIZE_A4_300_WIDTH;
    }
}

void place_dots(DWORD pos_x, BYTE *offsets, DWORD bpl, BYTE *bitmap_data_rows)
{
    DWORD new_x = pos_x, new_y = 0;
    for (BYTE a = 0; a < sizeof(DWORD); ++a)
    {
        new_y += offsets[a];

        draw_point(
            new_x, new_y, PRIVATE_PALETE_ID,
            bpl, bitmap_data_rows);

#ifdef HOST_LINKED
        // fprintf(stderr, "point at: %u - %u;\n", new_x, new_y);
#endif
    }
}

void encode(BYTE secret, DWORD pos_x, DWORD pos_y, DWORD bpl, BYTE *bitmap_data_rows)
{
#ifndef SHELL
    DWORD encoded = get_hashtext(secret, pos_x, pos_y);
#else
    DWORD encoded = get_hashtext_wrapper(secret, pos_x, pos_y);
#endif
    place_dots(pos_x, (BYTE *)&encoded, bpl, bitmap_data_rows);
}

extern void PSRFUNC *memcpy(void *src_, const void *dst_, size_t size)
{
    BYTE *src = (BYTE *)src_, *dst = (BYTE *)dst_;

    for (size_t a = 0; a < size; ++a)
    {
        dst[a] = src[a];
    }
}
