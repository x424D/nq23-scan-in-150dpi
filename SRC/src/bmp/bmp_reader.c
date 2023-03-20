#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bmp.h"

void display_bmp(struct BITMAP_INFO *info);
void output_palete(struct BITMAP_INFO *info, char *path);

int main(int argc, char *argv[])
{
    char *path_image = NULL,
         *path_output_palete = NULL;

    if (argc < 2)
    {
        fprintf(stderr, "specify path to DOS bmp file [path to output palete (optional)]\n");
        return EXIT_FAILURE;
    }

    if (argc > 3)
    {
        printf("going to put palete to: %s;\n", argv[2]);
    }

    path_image = argv[1];
    path_output_palete = argv[2];

    FILE *f_image = fopen(path_image, "rb");

    if (f_image == NULL)
    {
        fprintf(stderr, "failed to open: %s;\n", path_image);
        return EXIT_FAILURE;
    }

    struct BITMAP_INFO struct_bitmap;
    memset(&struct_bitmap, 0x00, sizeof(struct BITMAP_INFO));

    size_t res = fread(&struct_bitmap, 1, sizeof(struct BITMAP_INFO), f_image);
    printf("read %s: %lu / %lu\n", path_image, res, sizeof(struct BITMAP_INFO));

    display_bmp(&struct_bitmap);

    if (path_output_palete != NULL)
    {
        output_palete(&struct_bitmap, path_output_palete);
    }

    return EXIT_SUCCESS;
}

const char *unsigned_print_format = "%20s:\t%x (%u);\n";

#define PRINT_UNSIGNED_INTRO_FIELD(M_name, M_field)                                      \
    {                                                                                    \
        printf(unsigned_print_format, M_name, info->intro.M_field, info->intro.M_field); \
    }
#define PRINT_UNSIGNED_HEADER_FIELD(M_name, M_field)                                               \
    {                                                                                              \
        printf(unsigned_print_format, M_name, info->infoheader.M_field, info->infoheader.M_field); \
    }
#define PRINT_UNSIGNED(M_description, M_value)                          \
    {                                                                   \
        printf(unsigned_print_format, M_description, M_value, M_value); \
    }

void display_bmp(struct BITMAP_INFO *info)
{
    printf(unsigned_print_format, "intro size", sizeof(struct BITMAP_INTRO), sizeof(struct BITMAP_INTRO));
    PRINT_UNSIGNED_INTRO_FIELD("signature", bfType);
    PRINT_UNSIGNED_INTRO_FIELD("file size", bfSize);
    PRINT_UNSIGNED_INTRO_FIELD("offset to data", bfOffBits);

    printf(unsigned_print_format, "header size", sizeof(struct BITMAP_INFOHEADER), sizeof(struct BITMAP_INFOHEADER));
    PRINT_UNSIGNED_HEADER_FIELD("header size", biSize);
    PRINT_UNSIGNED_HEADER_FIELD("image width", biWidth);
    PRINT_UNSIGNED_HEADER_FIELD("image height", biHeight);

    PRINT_UNSIGNED_HEADER_FIELD("color planes", biPlanes);
    PRINT_UNSIGNED_HEADER_FIELD("bits per pixel", biBitCount);

    PRINT_UNSIGNED_HEADER_FIELD("compression", biCompression);
    PRINT_UNSIGNED_HEADER_FIELD("colors count", biClrUsed);

    DWORD bpl = get_row_size(info->infoheader.biWidth, info->infoheader.biBitCount);
    PRINT_UNSIGNED("bytes per line", bpl);
    PRINT_UNSIGNED_HEADER_FIELD("image size", biSizeImage);
    printf("%20s:\t%u;\n", "width x height",
           info->infoheader.biWidth * info->infoheader.biHeight);
    printf("%20s:\t%u;\n", "row x height",
           bpl * info->infoheader.biHeight);

    PRINT_UNSIGNED("total header size",
                   (sizeof(struct BITMAP_INTRO) + sizeof(struct BITMAP_INFOHEADER)));

    printf("\npalete:\n");
    print_palete(info->palete);
}

void output_palete(struct BITMAP_INFO *info, char *path)
{
    FILE *f_out = fopen(path, "wb+");
    if (f_out == NULL)
    {
        fprintf(stderr, "failed to open file for palete: %s;\n", path);
        return;
    }

    fwrite(info->palete, BITMAP_PALETE_SIZE, 1, f_out);
    printf("extracted palete to: %s;\n", path);
}