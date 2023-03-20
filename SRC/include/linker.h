#ifndef PSR_LINKER_
#define PSR_LINKER_

/*
    MACRO for linker
*/
#ifdef SHELL
#define PSRFUNC __attribute__((section(".shell")))
#define STATICVAR __attribute__((section(".static")))
#define INLINEFUNC inline __attribute__((always_inline))
#define NOGOT __attribute__((visibility("hidden")))
#else
#define PSRFUNC
#define STATICVAR
#define INLINEFUNC inline
#define NOGOT
#endif

#endif