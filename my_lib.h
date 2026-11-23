#ifndef STOP_ASSERTS
    #define my_assert(exp, ret) if(!(exp)) {\
    \
    printf("\x1b[33m" "%s:%d: [Assertation failed: %s]\n" "\x1b[0m", __FILE__, __LINE__, #exp);\
    return ret;}    

    
    #define my_assert_tell(exp, text, ret) if(!(exp)) {\
    \
    printf("\x1b[33m" "%s:%d: [Assertation failed: %s]\n" "\x1b[0m", __FILE__, __LINE__, text);\
    return ret;} 
#else
    #define my_assert(exp, ret) ;
    #define my_assert_tell(exp, text, ret) ;
#endif

#ifndef STOP_CHECK_POINTS
    #define CHECK_POINT printf("\x1b[35m" "CHECK_POINT: %s:%d\n" "\x1b[0m", __FILE__, __LINE__);
#else
    #define CHECK_POINT ;
#endif


#define BLACK "\x1b[30m"
#define RED "\x1b[31m"
#define GREEN "\x1b[32m"
#define YELLOW "\x1b[33m"
#define BLUE "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN "\x1b[36m"
#define WHITE "\x1b[37m"
#define RESET "\x1b[0m"