#include "types.h"

enum POINT_DESCRIPTION
{
    undefined = 0
};

struct POINT
{
    WORD x;
    WORD y;
    enum POINT_DESCRIPTION description;
};

#define MAX_POINTS_PER_VALUE 8
#define RANGE 1240 //

struct ENCODED_VALUE
{
    WORD count;
    struct POINT *points[MAX_POINTS_PER_VALUE];
};

int encode(
    struct POINT *original_position,
    SECRET secret,
    struct ENCODED_VALUE *output);