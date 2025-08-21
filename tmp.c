#include <stdio.h>


#define CHECK(...)          CHECK_N(__VA_ARGS__, 0,)
#define CHECK_N(x, n, ...)  n
#define PROBE(x)            x, 1,

#define IS_5(x)         CHECK(IS_##x##_PROBE)
#define IS_5_PROBE(...) PROBE(~)

#define IS_PAREN(x)             CHECK(IS_PAREN_PROBE x)
#define IS_PAREN_PROBE(...)     PROBE(~)

int main()
{
    CHECK(xxx);
    CHECK(PROBE(~));

    IS_5(5);
    IS_5(12);

    IS_PAREN(());
    IS_PAREN(foo);

    return (0);
}
