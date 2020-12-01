#ifndef __DEBUG_H__
#define __DEBUG_H__

#ifdef DEBUG
    #include <stdio.h>

    // ANSI Color stuff
    #define ANSI(n) "\033["#n"m"
    #define RESET ANSI(0)
    #define BOLD ANSI(1)
    #define CLEAR "\033[1K"
    #define RED     ANSI(31)
    #define GREEN   ANSI(32)

    #ifndef MAX_ALLOCS
        #define MAX_ALLOCS 1000
    #endif

    #ifndef DEBUG_NO_COLORS
        #define __PRINTLN_GREEN(format, args...) printf(GREEN format RESET "\n", args)
        #define __PRINTLN_RED_BOLD(format, args...) printf(RED BOLD format RESET "\n", args)
    #else
        #define __PRINTLN_GREEN(format, args...) printf(format "\n", args)
        #define __PRINTLN_RED_BOLD(format, args...) printf(format "\n", args)
    #endif
        

    typedef struct {
        void *__ptr;
        size_t __size;
        char *__file;
        int __line;
        char *__free_file;
        int __freed;
    } __alloc_t;
    extern __alloc_t __allocs[MAX_ALLOCS];
    extern int __nallocs;
    
    #define __ORIG_MALLOC(size) malloc(size)
    #define __ORIG_FREE(ptr) free(ptr)

    #define __MK_ALLOC_T(at, ptr, size, file, line) ({ \
        at.__ptr = ptr;\
        at.__size = size;\
        at.__file = file;\
        at.__line = line;\
        at.__freed = 0;\
        at.__free_file = "";\
    })

    #define __UPDATE_ALLOC_T(old, newptr, size) ({ \
        if (old == NULL) {\
            __MK_ALLOC_T(__allocs[__nallocs], newptr, size, __FILE__, __LINE__);\
            __allocs[++__nallocs].__ptr = NULL;\
        } else {\
            int __i;\
            for (__i = 0; __i < __nallocs && old != __allocs[__i].__ptr; __i++);\
            if (__i < __nallocs){\
                __allocs[__i].__ptr = newptr;\
                __allocs[__i].__size = size;\
            }\
        }\
        1;\
    })

    #define __FREE_ALLOC_T(ptr, file, line) ({ \
        int __i;\
        for (__i = 0; __i < __nallocs && (ptr != __allocs[__i].__ptr || __allocs[__i].__freed); __i++);\
        if (__i < __nallocs) {\
            __allocs[__i].__freed = line;\
            __allocs[__i].__free_file = file;\
        }\
        1;\
    })

    #define malloc(size) ({ \
        void *__ptr = malloc((size));\
        printf("Allocated: %ld bytes to %p in (%s:%d)\n", (size), __ptr, __FILE__, __LINE__);\
        __MK_ALLOC_T(__allocs[__nallocs], __ptr, size, __FILE__, __LINE__);\
        __allocs[++__nallocs].__ptr = NULL;\
        __ptr;\
    })

    #define calloc(nmemb, size) ({ \
        void *__ptr = calloc(nmemb, size);\
        printf("Allocated (calloc): %ld bytes to %p in (%s:%d)\n", (size * nmemb), __ptr, __FILE__, __LINE__);\
        __MK_ALLOC_T(__allocs[__nallocs], __ptr, size * nmemb, __FILE__, __LINE__);\
        __allocs[++__nallocs].__ptr = NULL;\
        __ptr;\
    })

    #define realloc(ptr, newsize) ({ \
        void *__orig_ptr = ptr;\
        void *__ptr = realloc(__orig_ptr, newsize);\
        __UPDATE_ALLOC_T(__orig_ptr, __ptr, newsize);\
        printf("Reallocated: %p -> %p to have %ld bytes in (%s:%d)\n", __orig_ptr, __ptr, (newsize), __FILE__, __LINE__);\
        __ptr;\
    })
    
    #define free(ptr) ({ \
        void *__orig_ptr = ptr;\
        printf("Freed: %p in (%s:%d)\n", __orig_ptr, __FILE__, __LINE__);\
        __FREE_ALLOC_T(__orig_ptr, __FILE__, __LINE__);\
        free(__orig_ptr);\
    })

    #define print_debug_info() ({ \
        printf("================= SUMMARY =================\n");\
        for (int i = 0; i < MAX_ALLOCS && __allocs[i].__ptr != NULL; i++)\
            if (__allocs[i].__freed)\
                __PRINTLN_GREEN("Allocated: %p at (%s:%d) and freed at (%s:%d)",\
                       __allocs[i].__ptr, __allocs[i].__file, __allocs[i].__line,\
                       __allocs[i].__free_file, __allocs[i].__freed);\
            else\
                __PRINTLN_RED_BOLD("Allocated: %p at (%s:%d) and was not freed!",\
                       __allocs[i].__ptr, __allocs[i].__file, __allocs[i].__line);\
        printf("===========================================\n");\
    })

    #ifdef MAIN_DEBUG
        __alloc_t __allocs[MAX_ALLOCS];
        int __nallocs;

        extern void __attribute__ ((constructor)) init_debug_vars() {
            __nallocs = 0;
        }

        extern void __attribute__ ((destructor)) print_debug() {
            print_debug_info();
        }
    #endif /* MAIN_DEBUG */
#else /* DEBUG */
    #define print_debug_info()
#endif /* DEBUG */

#endif /* __DEBUG_H__ */
