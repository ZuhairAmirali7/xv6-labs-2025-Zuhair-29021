#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    return 0;
}

int match(char *re, char *text);
int matchhere(char*, char*);
int matchstar(int, char*, char*);

int match(char *re, char *text)
{
    if(re[0] == '^') return matchhere(re+1, text);
    do {
        if(matchhere(re, text)) return 1;
    } while(*text++ != '\0');
    return 0;
}

int matchhere(char *re, char *text)
{
    if(re[0] == '\0') return 1;
    if(re[1] == '*') return matchstar(re[0], re+2, text);
    if(re[0] == '$' && re[1] == '\0') return *text == '\0';
    if(*text != '\0' && (re[0] == '.' || re[0] == *text))
        return matchhere(re+1, text+1);
    return 0;
}

int matchstar(int c, char *re, char *text)
{
    do {
        if(matchhere(re, text)) return 1;
    } while(*text != '\0' && (*text++ == c || c == '.'));
    return 0;
}
