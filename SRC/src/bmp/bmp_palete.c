#include "bmp.h"
#include "argv_help.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

struct RGBA32_DIFF
{
    float b;
    float g;
    float r;
    float a;
};
typedef struct RGBA32_DIFF BITMAP_PALETE_DIFF;

void help()
{
    printf(
        "./this [command]\n"
        "[target path] - path to output/file to modify\n"
        "[command] goes:\n"
        "\tnew [target path] [mode] - create new palete\n"
        "\tmod [target path] [mode] - change palete\n"
        "\tshow [target path] - show palete\n"
        "\treplace [palete path] [target path] - replace palete\n"
        "[mode] goes:\n"
        "\t(range/id) - \'[#ID1 #ID2]\', \'#ID1\', \'all\'\n"
        "\t(operation) - ariphmetical operation to apply\n"
        "\t(value) - [R1 G1 B1, R2 G2 B2]\n");
}

void exit_help(const char *func, const char *message)
{
    fprintf(stderr, "# %10s: %s\n", func, message);
    help();
    exit(1);
}

typedef void (*fun_bitemod)(BITMAP_PALETE_ITEM *a, BITMAP_PALETE_DIFF diff);

void fun_bitemod_set(BITMAP_PALETE_ITEM *a, BITMAP_PALETE_DIFF diff)
{
    BYTE
        r = (BYTE)diff.r,
        g = (BYTE)diff.g,
        b = (BYTE)diff.b;

    if (diff.r < 0)
        r = 0;
    if (diff.g < 0)
        g = 0;
    if (diff.b < 0)
        b = 0;

    if (diff.r > 0xFF)
        r = 0xFF;
    if (diff.g > 0xFF)
        g = 0xFF;
    if (diff.b > 0xFF)
        b = 0xFF;

    a->r = r;
    a->g = g;
    a->b = b;
}

struct operation
{
    FILE *f_target;

    DWORD id_start;
    DWORD id_end;
    DWORD size;

    fun_bitemod op;
    BITMAP_PALETE_ITEM data[BITMAP_PALETE_ITEMS_COUNT];

    BITMAP_PALETE_DIFF delta;
    BITMAP_PALETE_DIFF current_diff;
};

int parse_mode(int argc, char *argv[], struct operation *op)
{
    ARGCCHECK(argc, 3)
    char *range = argv[0];
    char *sign = argv[1];
    char *delta = argv[2];

    DWORD t_start = 0, t_end = 0, t_size = 0;

    if (STREQ(range, "all"))
    {
        t_start = 0;
        t_end = BITMAP_PALETE_ITEMS_COUNT;
        t_size = t_end - 1;
    }
    else if (*range == '[')
    {
        sscanf(range, "[%u %u]", &t_start, &t_end);
        t_size = t_end - 1;
    }
    else
    {
        sscanf(range, "%x", &t_start);
        t_end = t_start + 1;
        t_size = 1;
    }

    printf("range: %u - %u;\n", t_start, t_end);
    if (t_start >= t_end || t_end > BITMAP_PALETE_ITEMS_COUNT)
    {
        EXIT_HELP("incorrect range");
    }

    op->id_start = t_start;
    op->id_end = t_end;
    op->size = t_size;

    if (*sign == '=')
    {
        op->op = &fun_bitemod_set;
    }

    int r1, g1, b1, r2, g2, b2;
    sscanf(delta, "[%2x %2x %2x, %2x %2x %2x]",
           &r1, &g1, &b1, &r2, &g2, &b2);

    op->delta.a = 0;
    op->delta.r = (float)(r2 - r1) / op->size;
    op->delta.g = (float)(g2 - g1) / op->size;
    op->delta.b = (float)(b2 - b1) / op->size;

    op->current_diff.a = 0;
    op->current_diff.r = r1;
    op->current_diff.g = g1;
    op->current_diff.b = b1;

    printf("deltas:\n");
    printf("%f - %x: %f;\n", op->current_diff.r, r2, op->delta.r);
    printf("%f - %x: %f;\n", op->current_diff.g, g2, op->delta.g);
    printf("%f - %x: %f;\n", op->current_diff.b, b2, op->delta.b);

    return EXIT_SUCCESS;
}

int parse_arguments(int argc, char *argv[], struct operation *op)
{
    ARGCCHECK(argc, 2)
    int current_step = EXIT_SUCCESS;
    bool do_parse_mode = false;

    if (STREQ(argv[1], "new"))
    {
        ARGCCHECK(argc, 3)
        char *path_target = argv[2];
        printf("new palete at: %s;\n", path_target);
        op->f_target = fopen(path_target, "wb+");
        memset(op->data, 0x00, BITMAP_PALETE_SIZE);
        do_parse_mode = true;
    }
    else if (STREQ(argv[1], "mod"))
    {
        ARGCCHECK(argc, 3)
        char *path_target = argv[2];
        printf("mod palete at: %s;\n", path_target);
        op->f_target = fopen(path_target, "rb+");
        fread(op->data, BITMAP_PALETE_SIZE, 1, op->f_target);
        do_parse_mode = true;
    }

    else if (STREQ(argv[1], "show"))
    {
        ARGCCHECK(argc, 3)
        char *path_target = argv[2];
        printf("show palete at: %s;\n", path_target);
        op->f_target = fopen(path_target, "rb");
        fread(op->data, BITMAP_PALETE_SIZE, 1, op->f_target);

        print_palete(op->data);
        exit(0);
    }
    else if (STREQ(argv[1], "replace"))
    {
        ARGCCHECK(argc, 4)
        char *path_palete = argv[2];
        FILE *f_palete = fopen(path_palete, "rb");
        fread(op->data, BITMAP_PALETE_SIZE, 1, f_palete);

        char *path_target = argv[3];
        FILE *f_target = fopen(path_target, "rb+");
        fseek(f_target, BITMAP_PALETE_OFFSET, SEEK_SET);
        fwrite(op->data, BITMAP_PALETE_SIZE, 1, f_target);

        printf("replaced pelete at: %s (0x%lx);\nwith palete from: %s;\n",
               path_target, BITMAP_PALETE_OFFSET, path_palete);

        exit(0);
    }

    if (do_parse_mode)
    {
        if (parse_mode(argc - 3, argv + 3, op))
        {
            EXIT_HELP("failed to parse mode");
        }
    }
}

void run(struct operation *op)
{
    DWORD a = op->id_start;
    for (; a < op->id_end; ++a)
    {
        op->op(op->data + a, op->current_diff);
        op->current_diff.r += op->delta.r;
        op->current_diff.g += op->delta.g;
        op->current_diff.b += op->delta.b;

        // printf("[%3d] diff: %f;\n", a, op->current_diff.r);
    }

    printf("done items: [%u, %u];\n", op->id_start, a);

    rewind(op->f_target);
    fwrite(op->data, BITMAP_PALETE_SIZE, 1, op->f_target);
    // print_palete(op->data);
}

int main(int argc, char *argv[])
{
    struct operation op;
    parse_arguments(argc, argv, &op);
    run(&op);

    return 0;
}