#ifndef UTIL_H
#define UTIL_H

#define START_ANSI "\x1B["
#define END_ANSI "\x1B[0m"

static inline void print_red(char* text){
    printf(START_ANSI "31m%s" END_ANSI, text);
}

static inline void print_green(char* text){
    printf(START_ANSI "32m%s" END_ANSI, text);
}

static inline void print_yellow(char* text){
    printf(START_ANSI "33m%s" END_ANSI, text);
}

static inline void print_blue(char* text){
    printf(START_ANSI "34m%s" END_ANSI, text);
}

static inline void print_magenta(char* text){
    printf(START_ANSI "35m%s" END_ANSI, text);
}

static inline void print_cyan(char* text){
    printf(START_ANSI "36m%s" END_ANSI, text);
}

#endif //UTIL_H