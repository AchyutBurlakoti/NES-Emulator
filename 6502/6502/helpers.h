#pragma once

#include <utility>

template <typename F>
struct DEFER {
    DEFER(F f) : f(f) {}
    ~DEFER() { f(); }
    F f;
};

template <typename F>
DEFER<F> makedefer(F f) {
    return DEFER<F>(f);
};

#define __defer( line ) defer_ ## line
#define _defer( line ) __defer( line )

struct defer_dummy { };

template<typename F>
DEFER<F> operator+(defer_dummy, F&& f)
{
    return makedefer<F>(std::forward<F>(f));
}

#define defer auto _defer( __LINE__ ) = defer_dummy( ) + [ & ]( )


// Macro that gives back a format specifier for a type
 #define FMTSPEC(x) _Generic((x), int: "%d", \
                                    long: "%ld", \
                                        float: "%f", \
                                            double: "%f", char *: "%s")
                                            // TODO: add more types

 // Macro that prints a variable in the form "name = value"
 #define PRINT_VAL(x) do { \
             char fmt[512]; \
                 snprintf(fmt, sizeof fmt, #x " = %s\n", FMTSPEC(x)); \
                     printf(fmt, (x)); \
                         }while(0)





namespace type
{
    typedef union
    {
        struct
        {
            uint16_t r;
            uint16_t g;
            uint16_t b;
        };

        struct
        {
            uint16_t x;
            uint16_t y;
            uint16_t z;
        };

        struct
        {
            uint16_t a;
            uint16_t b;
            uint16_t c;
        };

    } RGB;
};

extern type::RGB SYSTEM_PALETTE[64];