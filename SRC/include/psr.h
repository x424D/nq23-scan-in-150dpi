#ifndef PSR_TEXT_
#define PSR_TEXT_

#ifdef HOST_LINKED
#include <stdio.h>
#endif

#include "linker.h"
#include "types.h"
#include "bmp.h"

void PSRFUNC entry_psr(WORD resolution, BYTE *buf_output);

/*

    @request_greyscale - this function is pseudo-hardware request for scanning
    it 'returns' 8-bit greyscale scanning result A4-300dpi

    player must hook this call and provide data for testing

    @static_buf_greyscale - this array is used with @request_greyscale
    to store greyscale

    at this point:
        - PSR: symbols must be mapped to some fixed address
        - HOST:
            - LINKED: provide symbols
            - STATIC: linked with symbols from psr_static

*/

extern void request_greyscale(BYTE *buf_greyscale);
#ifdef SHELL
#define static_buf_greyscale 0x10000000;
#else
extern BYTE static_buf_greyscale[BITMAP_MAX_RESOLUTION];
#endif
//

BYTE *PSRFUNC get_buf_greyscale();

///
extern const BYTE NOGOT STATICVAR static_bitmap_palete[BITMAP_PALETE_SIZE];

void PSRFUNC output_bitmap(WORD resolution, BYTE *buf_greyscale, BYTE *buf_output);

void PSRFUNC translate_greyscale_to_bmp(struct PICTURE_SIZE *sizes, BYTE *buf_greyscale, BYTE *bitmap_data_rows);

#define SECRET_CHECK_A_POS 0b00
#define SECRET_CHECK_B_POS 0b01
#define SECRET_CHECK_M_POS 0b10
#define SECRET_CHECK_S_POS 0b11

#define SECRET_POINT_OFFSET_A 0
#define SECRET_POINT_OFFSET_B PICSIZE_A4_300_WIDTH
#define SECRET_POINT_OFFSET_M 1
#define SECRET_POINT_OFFSET_S (PICSIZE_A4_300_WIDTH + 1)

#define SECRET_HOLDER_BOTTOM 0x40

void PSRFUNC downgrade_greyscale_to_bmp(struct PICTURE_SIZE *sizes, BYTE *buf_greyscale, BYTE *bitmap_data_rows);

void *PSRFUNC memcpy(void *src, const void *dst, size_t size);

///
bool PSRFUNC is_secret_holder(BYTE *point_values);
size_t PSRFUNC count_secret_holders();
void PSRFUNC get_secret_holder_by_id(size_t id, DWORD *value_holder);

///

void PSRFUNC encode(BYTE secret, DWORD pos_x, DWORD pos_y, DWORD bpl, BYTE *bitmap_data_rows);
void PSRFUNC place_dots(DWORD pos_x, BYTE *offsets, DWORD bpl, BYTE *bitmap_data_rows);
#ifndef SHELL
DWORD PSRFUNC emplace_opentext(BYTE secret, DWORD pos_x, DWORD pos_y);
#else
// is inline
#endif
/*
    @get_hashtext - encodes value

    this function should be modified during compilation to make it (little) harder to reverce
*/
DWORD PSRFUNC get_hashtext(BYTE secret, DWORD pos_x, DWORD pos_y);

#ifdef SHELL
extern DWORD SIZE_get_hashtext;
extern DWORD CHECKSUM_open_get_hashtext;
// extern void *_trash_0;
DWORD PSRFUNC get_hashtext_wrapper(BYTE secret, DWORD pos_x, DWORD pos_y);
#endif

#endif
