#include "BlackMagics.h"
#include <chrono>
#include <thread>

#if defined(_MSC_VER) && _MSC_VER >= 1310 && ( defined(_M_IX86) || defined(_M_X64) )

extern "C" void _mm_pause();

#define CALL_SMT_PAUSE _mm_pause();

#elif defined(__GNUC__) && ( defined(__i386__) || defined(__x86_64__) )

#define CALL_SMT_PAUSE __asm__ __volatile__( "rep; nop" : : : "memory" );

#endif

UTILS_EXPORTS_API void DefaultBlackMagicFunc(unsigned count)
{
    if (count < 4)
    {
    }
#if defined( CALL_SMT_PAUSE )
    else if (count < 16)
    {
        CALL_SMT_PAUSE
    }
#endif
    else if (count < 32)
    {
        std::this_thread::yield();
    }
    else if (count < 64)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    else
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}