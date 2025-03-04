#include <ncurses.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_WORD_LEN 50
#define INITIAL_WORDS_SIZE 10

int main(void)
{
    initscr();
    raw();
    keypad(stdscr, TRUE);
    noecho();
    curs_set(1); 

    const char *path = "words.txt";
    FILE *file = fopen(path, "r");

    if (file == NULL)
    {
        perror("Error Opening File");
        return 1;
    }

    char *buffer = (char *)malloc(INITIAL_WORDS_SIZE * sizeof(char));

    if (buffer == NULL)
    {
        perror("Error: Failed to allocate needed memory");
        fclose(file);  
        return 1;
    }

    size_t size = INITIAL_WORDS_SIZE;
    size_t buffer_length = 0;

    while (fgets(buffer, size, file) != NULL)
    {
        size_t length = strlen(buffer);
        if (buffer[length - 1] == '\n')
        {
            buffer[length - 1] = '\0';
            length--;
        }
        attron(A_DIM);
        printw("%s ", buffer);  
        attroff(A_DIM);
        refresh();
    }
    fclose(file); 

	int height, width;
    getmaxyx(stdscr, height, width);
	int start_x = 0;
	int start_y = 0;
	WINDOW *typing_win = newwin(height, width, start_y, start_x);

    wmove(typing_win, 0, 0);
	refresh();
    wrefresh(typing_win);

    char typed_words[1024] = {0};
    int index = 0;
    int ch;

    while ((ch = getch()) != '\n')
    {
        if (ch == KEY_BACKSPACE || ch == 127) 
        {
            if (index > 0)
            {
                index--;
                typed_words[index] = '\0'; 
                wmove(typing_win, 0, index); 
                wdelch(typing_win);
            }
        }
        else if (index < 1024 - 1) 
        {
            typed_words[index++] = ch;
            waddch(typing_win, ch); 
        }
		refresh();
        wrefresh(typing_win);
    }

    typed_words[index] = '\0'; 
    wmove(typing_win, 2, 0); 
    wprintw(typing_win, "You typed: %s", typed_words); 
    wrefresh(typing_win);

    wgetch(typing_win);
	delwin(typing_win);
    endwin(); 
    free(buffer); 

    return 0;
}
