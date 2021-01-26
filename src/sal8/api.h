#ifndef SAL8_API_H_GUARD
#define SAL8_API_H_GUARD

#ifdef TARGET_TYPE_LIBRARY
    #if defined _WIN32 || defined __CYGWIN__
        #ifdef __GNUC__
            #define SAL8_API __attribute__ ((dllexport))
        #else
            #define SAL8_API __declspec(dllexport)
        #endif

        #define SAL8_API_HIDDEN
    #else
        #if __GNUC__ >= 4
            #define SAL8_API __attribute__ ((visibility ("default")))
            #define SAL8_API_HIDDEN  __attribute__ ((visibility ("hidden")))
        #else
            #define SAL8_API
            #define SAL8_API_HIDDEN
        #endif
    #endif
#else
    #define SAL8_API
    #define SAL8_API_HIDDEN
#endif

#endif
