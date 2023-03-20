#include "psr.h"

inline __attribute__((always_inline)) DWORD murmur_32_scramble(DWORD k)
{
    k *= 0xcc9e2d51;
    k = (k << 15) | (k >> 17);
    k *= 0x1b873593;
    return k;
}

// murmur3_32
inline __attribute__((always_inline)) DWORD hash(DWORD value)
{
    DWORD seed = 0x333;

    uint32_t h = seed;
    uint32_t k = value;

    DWORD mumur = murmur_32_scramble(k);

    h ^= mumur;

    h = (h << 13) | (h >> 19);
    h = h * 5 + 0xe6546b64;

    h ^= sizeof(DWORD);
    h ^= h >> 16;
    h *= 0x85ebca6b;
    h ^= h >> 13;
    h *= 0xc2b2ae35;
    h ^= h >> 16;
    return h;
}

#ifdef SHELL
inline __attribute__((always_inline)) DWORD
#else
DWORD
#endif
emplace_opentext(BYTE secret, DWORD pos_x, DWORD pos_y)
{
    DWORD opentext = 0;

    BYTE bitwidth_pos = 12;
    WORD bitmask_pos = (1 << bitwidth_pos) - 1;

    DWORD shifted_pos_x = pos_x & bitmask_pos;
    opentext |= shifted_pos_x;

    DWORD shifted_secret = secret << bitwidth_pos;
    opentext |= shifted_secret;

    DWORD shifted_pos_y = (pos_y & bitmask_pos) << (bitwidth_pos + 8);
    opentext |= shifted_pos_y;

    return opentext;
}

#ifdef SHELL
__attribute__((aligned(0x400)))
#endif
DWORD
get_hashtext(BYTE secret, DWORD pos_x, DWORD pos_y)
{
    /*
        0'                              32'
        |-------=-------=-------=-------=|
        |-------=---1-------2---=-------=|

        0 - 1 (24)  = pos_x
        1 - 2 (8)   = secret
        2 - 3 (24)  = pos_y
        with dpi 300 position is limited with 12 bits
    */
    DWORD to_encode = emplace_opentext(secret, pos_x, pos_y);
    DWORD res = hash(to_encode);
    return res;
}
#ifdef SHELL
__attribute__((aligned(0x400))) DWORD get_hashtext_wrapper(BYTE secret, DWORD pos_x, DWORD pos_y)
{
    BYTE *p_target = (BYTE *)&get_hashtext,
         *p_decoding = (BYTE *)&get_hashtext,
         decoder = 0;

    DWORD sum = 0;

        // try to fool disassembler
    __asm__ volatile(
        "_trash_0_start: b _trash_0_hook;\n"
        "_trash_0_exit: \n"
        "mfspr 9, 8;\n"
        "addi 9, 9, 4;\n"
        "mtspr 8, 9;\n"
        "blr;\n;"
        "_trash_0_hook: bl _trash_0_exit;\n"
        "_trash_0_fix_on_rt: b _trash_0_start\n");

    for (DWORD a = 0; a < (DWORD)&SIZE_get_hashtext; ++a)
    {
        sum += p_target[a];
    }
    if (sum != (DWORD)&CHECKSUM_open_get_hashtext)
    {
        for (DWORD a = 0; a < (DWORD)&SIZE_get_hashtext; ++a)
        {
            if (a % 4 == 0)
            {
                decoder = *p_decoding;
                ++p_decoding;
            }

            p_target[a] = *p_decoding ^ decoder;
            ++p_decoding;
        }
    }

    DWORD res = get_hashtext(secret, pos_x, pos_y);
}
#endif
