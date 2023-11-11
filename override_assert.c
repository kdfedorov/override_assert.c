// override_assert.c
// gcc -g3 -wall -wpedantic -wextra -werror -o assert override_assert.c

// Looks like the only way to properly debug an asserts with gdb 
// is to not use stdlib vanilla asserts...

#include <assert.h>
#include <ctype.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#define USE_CUSTOM_ASSERT

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Source: https://stackoverflow.com/questions/35578614/how-to-override-assert-macro-in-c

/*!
 * Overwrite the standard (weak) definition of the assert failed handling function.
 *
 * These functions are called by the assert() macro and are named differently and
 * have different signatures on different systems.
 * - On Cygwin/Windows its   __assert_func()
 * - On Linux its            __assert_fail()
 *
 * - Output format is changed to reflect the gcc error message style
 *
 * @param filename    - the filename where the error happened
 * @param line        - the line number where the error happened
 * @param assert_func - the function name where the error happened
 * @param expr        - the expression that triggered the failed assert
 */
#if defined( __CYGWIN__ )

void __assert_func( const char *filename, int line, const char *assert_func, const char *expr )

#elif defined( __linux__ )

void __assert_fail ( const char* expr, const char *filename, unsigned int line, const char *assert_func )

#else
# error "Unknown OS! Don't know how to overwrite the assert failed handling function. Follow assert() and adjust!"
#endif
{
    // gcc error message style output format:
    fprintf( stdout, "%s:%d:4: error: assertion \"%s\" failed in function %s.\n",
             filename, line, expr, assert_func );

    abort();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef USE_CUSTOM_ASSERT

// Kinda Windows assert style.
void custom_assert_func( const char* expr, const char* filename, unsigned int line, const char* assert_func )
{
    // gcc error message style output format:
    fprintf( stdout, "%s:%d:4: error: assertion \"%s\" failed in function \"%s\".\n",
             filename, line, expr, assert_func );

    fprintf( stdout, "(A)bort, (B)reak, (C)ontinue: ");

    int c = toupper( getchar() );
    if (c == 'A')
        abort();
    else if (c == 'B')
        // This signals debugger to break...
        // For Windows: use DebugBreak or __debugbreak.
        raise(SIGTRAP);
}

#undef assert
#define assert(expr)                    \
    ((expr)								\
     ? (void)(0)			            \
     : custom_assert_func(#expr, __FILE__, __LINE__, __ASSERT_FUNCTION))

#endif

// Returns sum of all numbers from 0 to 'to'.
int arithprog(int to)
{
    assert(to >= 0 && "Only positive numbers are allowed");

    int sum = 0;
    for (int i = 0; i != to; ++i)
        sum += i;

    return sum;
}

int main(void)
{
    printf( "arithprog(10) = %d\n", arithprog(10) ); // OK
    printf( "arithprog(-4) = %d\n", arithprog(-4) ); // Oh no...
    return EXIT_SUCCESS;
}

