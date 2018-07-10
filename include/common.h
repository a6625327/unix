#ifndef __COMMON_H__

#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

void perr_exit(const char *str){
    perror(str);
    exit(1);
}

#endif // !__COMMON_H__

