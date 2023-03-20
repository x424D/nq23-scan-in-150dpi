#ifndef ARGVH_
#define ARGVH_


#define STREQ(a, b) (!strcmp(a, b))
#define ARGCCHECK(a, b)                                      \
    {                                                        \
        if (a < b)                                           \
        {                                                    \
            exit_help(__func__, "incorrect argument count"); \
        }                                                    \
    }
#define EXIT_HELP(message) exit_help(__func__, message);

#define NULLASSERT(M_v, M_message) {\
    if ( M_v == NULL){\
        fprintf(stderr, "# %s\n", M_message);\
        return EXIT_FAILURE;\
    }\
}

#endif