#include "psr.h"
#include "argv_help.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

void help()
{
    printf(
        "./psr_tools [command]"
        "[command] goes:\n"
        "\tcheck [a] [b] [m] - whether can be used to store secret\n"
        "\tfind [path to greyscale] - list secrets from 300dpi greyscale\n"
        "\thide [path to greyscale] [x] [y] [secret] - place secret holder to greyscale\n"
        "\templace [x] [y] [secret] - show how options are emplaced in DWORD\n"
        "\thashtext [x] [y] [secret] - get encoded value\n"
        "\tenumhash - enumirate hashes to stdout\n");
}

void exit_help(const char *func, const char *message)
{
    fprintf(stderr, "# %10s: %s\n", func, message);
    help();
    exit(1);
}

void do_check(int argc, char *argv[])
{
    ARGCCHECK(argc, 3);
    DWORD value_holder;
    BYTE *values = (BYTE *)&value_holder;
    BYTE secret = 0;

    values[SECRET_CHECK_A_POS] = (BYTE)atoi(argv[0]);
    values[SECRET_CHECK_B_POS] = (BYTE)atoi(argv[1]);
    values[SECRET_CHECK_M_POS] = (BYTE)atoi(argv[2]);
    values[SECRET_CHECK_S_POS] = 0;
    if (argc > 3)
    {
        values[SECRET_CHECK_S_POS] = (BYTE)atoi(argv[3]);
    }

    printf("a: %u; b: %u; m: %u; s: %u; 0x%X\n",
           values[SECRET_CHECK_A_POS],
           values[SECRET_CHECK_B_POS],
           values[SECRET_CHECK_M_POS],
           values[SECRET_CHECK_S_POS],
           value_holder);

    if (is_secret_holder(values))
    {
        printf("is secret!\n");
    }
    else
    {
        printf("is not secret!\n");
    }
}

void list_usable_secrets()
{
    DWORD value_holder;
    BYTE *values = (BYTE *)&value_holder;
    BYTE secret;
    size_t found_secrets = 0;
    for (value_holder = 0; value_holder < 0xFFFFFF; ++value_holder)
    {
        if (is_secret_holder(values))
        {
            fprintf(stderr,
                    "\n%X\n", value_holder);
            printf("%2x %2x %2x\n",
                   values[SECRET_CHECK_A_POS],
                   values[SECRET_CHECK_B_POS],
                   values[SECRET_CHECK_M_POS]);
            fprintf(stderr,
                    "%3u %3u %3u\n",
                    values[SECRET_CHECK_A_POS],
                    values[SECRET_CHECK_B_POS],
                    values[SECRET_CHECK_M_POS]);
            ++found_secrets;
        }
    }
    fprintf(stderr, "\ntotal: %lu;\n", found_secrets);
}

void find_secrets(FILE *f_grescale)
{
    BYTE buf_greyscale_row[PICSIZE_A4_300_WIDTH * 2];
    DWORD point_values_holder = 0;
    BYTE *point_values = (BYTE *)&point_values_holder;
    BYTE secret;

    for (DWORD a = 0; a < PICSIZE_A4_300_HEIGHT; a += 2)
    {
        if (!fread(buf_greyscale_row, sizeof(BYTE),
                   PICSIZE_A4_300_WIDTH * 2, f_grescale))
        {
            fprintf(stderr, "failed to read gretscale at: %ul;\n", a);
        }

        for (DWORD b = 0; b < PICSIZE_A4_300_WIDTH; b += 2)
        {
            point_values[SECRET_CHECK_A_POS] = buf_greyscale_row[b];
            point_values[SECRET_CHECK_B_POS] = buf_greyscale_row[b + PICSIZE_A4_300_WIDTH];
            point_values[SECRET_CHECK_M_POS] = buf_greyscale_row[b + 1];
            point_values[SECRET_CHECK_S_POS] = buf_greyscale_row[b + PICSIZE_A4_300_WIDTH + 1];

            if (is_secret_holder(point_values))
            {
                printf("%u - %u: %x;\n", b, a, secret);
            }
        }
    }
}

void get_secret_holder(DWORD *secret_holder)
{
    static size_t secret_holders_available = 0;
    if (!secret_holders_available)
    {
        secret_holders_available = count_secret_holders();
    }

    size_t holder_id = rand() % secret_holders_available;
    // fprintf(stderr, "used id: %lu;\n", holder_id);
    get_secret_holder_by_id(holder_id, secret_holder);
}

void put_byte(FILE *f_greyscale, size_t offset, BYTE value)
{
    // fprintf(stderr, "put: %lu: %X;\n", offset, value);
    fseek(f_greyscale, offset, SEEK_SET);
    fwrite(&value, sizeof(BYTE), 1, f_greyscale);
}

void add_secret(FILE *f_greyscale, DWORD pos_x, DWORD pos_y, BYTE secret)
{
    srand(time(NULL));

    DWORD secret_holder = 0;
    BYTE *secret_values = (BYTE *)&secret_holder;
    get_secret_holder(&secret_holder);
    secret_values[SECRET_CHECK_S_POS] = secret;

    size_t offset_start = pos_y * PICSIZE_A4_300_WIDTH + pos_x;

    put_byte(f_greyscale, offset_start + SECRET_POINT_OFFSET_A, secret_values[SECRET_CHECK_A_POS]);
    put_byte(f_greyscale, offset_start + SECRET_POINT_OFFSET_B, secret_values[SECRET_CHECK_B_POS]);
    put_byte(f_greyscale, offset_start + SECRET_POINT_OFFSET_M, secret_values[SECRET_CHECK_M_POS]);
    put_byte(f_greyscale, offset_start + SECRET_POINT_OFFSET_S, secret_values[SECRET_CHECK_S_POS]);
}

void put_emplaced(DWORD pos_x, DWORD pos_y, BYTE secret)
{
    printf("0x%X\n", emplace_opentext(secret, pos_x, pos_y));
}

void put_hashtext(DWORD pos_x, DWORD pos_y, BYTE secret)
{
    printf("0x%X\n", get_hashtext(secret, pos_x, pos_y));
}

void enumhashes()
{
    for (DWORD y = 0; y < PICSIZE_A4_300_HEIGHT; ++y)
    {
        fprintf(stderr, "line: %u / %u;\n", y, PICSIZE_A4_300_HEIGHT);

        for (DWORD x = 0; x < PICSIZE_A4_300_HEIGHT; ++x)
        {
            for (WORD v = 0; v <= 0xFF; ++v)
            {
                DWORD hash = get_hashtext(v, x, y);
                fwrite(&hash, sizeof(DWORD), 1, stdout);
            }
        }
    }
}

int main(int argc, char *argv[])
{
    ARGCCHECK(argc, 2);
    char *command = argv[1];

    if (STREQ(command, "check"))
    {
        do_check(argc - 2, argv + 2);
    }
    else if (STREQ(command, "secrets"))
    {
        list_usable_secrets();
    }
    else if (STREQ(command, "find"))
    {
        ARGCCHECK(argc, 3);
        char *path_greyscale = argv[2];
        FILE *f_greyscale = fopen(path_greyscale, "rb");
        NULLASSERT(f_greyscale, "cannot open greyscale");

        find_secrets(f_greyscale);
    }
    else if (STREQ(command, "hide"))
    {
        ARGCCHECK(argc, 6);
        char *path_greyscale = argv[2];
        FILE *f_greyscale = fopen(path_greyscale, "rb+");
        NULLASSERT(f_greyscale, "failed to open greyscale");

        DWORD pos_x = strtoul(argv[3], NULL, 10);
        DWORD pos_y = strtoul(argv[4], NULL, 10);
        BYTE secret = strtoul(argv[5], NULL, 10);

        add_secret(f_greyscale, pos_x, pos_y, secret);
    }

    else if (STREQ(command, "emplace"))
    {
        ARGCCHECK(argc, 4);
        // emplace 2730 1365 2222
        DWORD pos_x = strtoul(argv[2], NULL, 10);
        DWORD pos_y = strtoul(argv[3], NULL, 10);
        BYTE secret = strtoul(argv[4], NULL, 10);

        put_emplaced(pos_x, pos_y, secret);
    }

    else if (STREQ(command, "hashtext"))
    {
        ARGCCHECK(argc, 4);
        // emplace 2730 1365 2222
        DWORD pos_x = strtoul(argv[2], NULL, 10);
        DWORD pos_y = strtoul(argv[3], NULL, 10);
        BYTE secret = strtoul(argv[4], NULL, 10);

        put_hashtext(pos_x, pos_y, secret);
    }
    else if (STREQ(command, "enumhash"))
    {
        enumhashes();
    }
    else
    {
        exit_help("entry", "incorrect command");
    }

    return EXIT_SUCCESS;
}

BYTE static_buf_greyscale[BITMAP_MAX_RESOLUTION] = {
    0,
};

void request_greyscale(BYTE *buf_greyscale)
{
    return;
}