#ifndef KERNEL_H
#define KERNEL_H

#include <types.h>

#define DEBUG 1
#if DEBUG == 0
    #define LOG(...)
#else
    #define LOG(...) printf(__VA_ARGS__);

#endif

s32 printf(char * format, ... );

force_inline void pcsx_putc(int c) {
    *((volatile char * const) 0x1f802080) = c;
}

force_inline void pcsx_debugbreak() {
    *((volatile char * const) 0x1f802081) = 0;
}

force_inline void pcsx_exit(int code) {
    *((volatile int16_t * const) 0x1f802082) = code;
}

force_inline void pcsx_message(char * msg) {
    *((volatile char ** const) 0x1f802084) = msg;
}

force_inline int pcsx_present() {
    return *((volatile uint32_t * const) 0x1f802080) == 0x58534350;
}

#endif