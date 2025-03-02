#include <stdio.h>
/* #include <ncurses.h> */

#define INITIAL_WORDS 10
#define MAX_WORD_LEN 50

int main(void) 
{
    char *path = "words.txt"; // when done add this to header file
    FILE *file = fopen(path, "r");
    return 0;
}
