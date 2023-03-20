#include "psr.h"

inline __attribute__((always_inline)) BYTE holder_evaluation_check(BYTE a, BYTE b, BYTE m)
{
    WORD r = a * (b - 1);
    return r % m;
}

bool is_secret_holder(BYTE *point_values)
{
    BYTE a = point_values[SECRET_CHECK_A_POS],
         b = point_values[SECRET_CHECK_B_POS],
         m = point_values[SECRET_CHECK_M_POS];
    //  s = point_values[SECRET_CHECK_S_POS];

    if (
        // !(a % 2) || !(b % 2) || !(m % 2) ||
        !(a & b & m & 0x01) ||
        a < SECRET_HOLDER_BOTTOM ||
        b < SECRET_HOLDER_BOTTOM ||
        m < SECRET_HOLDER_BOTTOM ||
        a == b || b == m || a == m)
    {
        return false;
    }

    if (!holder_evaluation_check(a, b, m) &&
        !holder_evaluation_check(b, a, m))
    {
        return true;
    }

    return false;
}

size_t count_secret_holders()
{
    DWORD value_holder;
    BYTE *values = (BYTE *)&value_holder;
    BYTE secret;
    size_t res = 0;

    for (value_holder = 0; value_holder < 0xFFFFFF; ++value_holder)
    {
        if (is_secret_holder(values))
        {
            ++res;
        }
    }

    if (!res)
    {
#ifdef HOST_LINKED
        fprintf(stderr, "\t\t!!NO SECRET HOLDERS!!\n");
        exit(0);
#else
        // 1 / 0;
#endif
    }

    return res;
}

void get_secret_holder_by_id(size_t id, DWORD *p_value_res)
{
    size_t current_id = 0;

    DWORD value_holder = 0;
    BYTE *values = (BYTE *)&value_holder;
    for (value_holder = 0; value_holder < 0xFFFFFF; ++value_holder)
    {
        if (is_secret_holder(values))
        {
            if (++current_id == id)
            {
                *p_value_res = value_holder;
                break;
            }
        }
    }
}