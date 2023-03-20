OUTPUT_FORMAT("elf32-powerpc", "elf32-powerpc",
              "elf32-powerpc")
OUTPUT_ARCH(powerpc:common)

INCLUDE src/psr/ppc-psr-compiletime-const.ld

ENTRY(entry_psr)


request_greyscale = 0x200000;
PATCH_OFFSET = 0x400000;


MEMORY {
    FLASH (rx)      : ORIGIN = PATCH_OFFSET, LENGTH = 1024K
}

SECTIONS
{   . = ORIGIN (FLASH);
    .patch : {
        FILL(0x00);
        KEEP(*(.shell))
        FILL(0x00);
        KEEP(*(.static))
        FILL(0x00);
    } > FLASH

}