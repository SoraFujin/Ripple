#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define MAX_WORD_LEN 50
#define MAX_WORDS 1000
#define MAX_DISPLAY_WORDS 100

typedef struct {
    char **words;
    int count;
} WordList;

void initWordList(WordList *list, int capacity) {
    list->words = (char**)malloc(capacity * sizeof(char*));
    list->count = 0;
}

void addWord(WordList *list, const char *word) {
    list->words[list->count] = strdup(word);
    list->count++;
}

void freeWordList(WordList *list) {
    for (int i = 0; i < list->count; i++) {
        free(list->words[i]);
    }
    free(list->words);
}

int main(void) {
    initscr();
    start_color();
    cbreak();
    keypad(stdscr, TRUE);
    noecho();
    curs_set(1);

    if (!has_colors()) {
        endwin();
        printf("Your terminal doesn't support colors\n");
        return 1;
    }

    init_pair(1, COLOR_WHITE, COLOR_BLACK);   // Untyped text
    init_pair(2, COLOR_GREEN, COLOR_BLACK);   // Correctly typed
    init_pair(3, COLOR_RED, COLOR_BLACK);     // Incorrectly typed
    init_pair(4, COLOR_YELLOW, COLOR_BLACK);  // Current position

    WordList wordList;
    initWordList(&wordList, MAX_WORDS);

    const char *path = "words.txt";
    FILE *file = fopen(path, "r");
    if (file == NULL) {
        endwin();
        perror("Error Opening File");
        return 1;
    }

    char buffer[MAX_WORD_LEN + 1];
    while (fgets(buffer, MAX_WORD_LEN, file) != NULL && wordList.count < MAX_WORDS) {
        size_t length = strlen(buffer);
        if (buffer[length - 1] == '\n') {
            buffer[length - 1] = '\0';
            length--;
        }

        if (length > 0) {
            addWord(&wordList, buffer);
        }
    }
    fclose(file);

    if (wordList.count == 0) {
        endwin();
        printf("No words loaded from file.\n");
        return 1;
    }

    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    WINDOW *typing_win = newwin(max_y, max_x, 0, 0);
    keypad(typing_win, TRUE);

    mvwprintw(typing_win, 1, max_x/2 - 10, "RIPPLE TYPING GAME");
    mvwprintw(typing_win, 2, 2, "Type the words as they appear. Press ESC to quit.");

    srand(time(NULL));

    int display_count = wordList.count < MAX_DISPLAY_WORDS ? wordList.count : MAX_DISPLAY_WORDS;
    char display_text[MAX_WORD_LEN * MAX_DISPLAY_WORDS] = {0};
    int display_indices[MAX_DISPLAY_WORDS];

    for (int i = 0; i < display_count; i++) {
        display_indices[i] = rand() % wordList.count;
        strcat(display_text, wordList.words[display_indices[i]]);
        strcat(display_text, " ");
    }
    display_text[strlen(display_text)-1] = '\0'; 

    int start_y = max_y / 2;
    int text_len = strlen(display_text);
    int line_width = max_x - 4;
    int lines_needed = (text_len + line_width - 1) / line_width;

    if (start_y + lines_needed + 5 > max_y) {
        start_y = max_y - lines_needed - 5;
    }

    wattron(typing_win, COLOR_PAIR(1));
    int curr_x = 2;
    int curr_y = start_y;

    for (int i = 0; i < text_len; i++) {
        if (curr_x >= max_x - 2) {
            curr_x = 2;
            curr_y++;
        }
        mvwaddch(typing_win, curr_y, curr_x++, display_text[i]);
    }
    wattroff(typing_win, COLOR_PAIR(1));

    int typed_len = 0;
    int cursor_x = 2;
    int cursor_y = start_y;

    wmove(typing_win, cursor_y, cursor_x);
    wrefresh(typing_win);

    int ch;
    int game_over = 0;

    while (!game_over && (ch = wgetch(typing_win)) != 27) { 
        if (ch == KEY_BACKSPACE || ch == 127) {
            if (typed_len > 0) {
                typed_len--;

                cursor_x--;
                if (cursor_x < 2) {
                    if (cursor_y > start_y) {
                        cursor_y--;
                        cursor_x = max_x - 2;
                    } else {
                        cursor_x = 2;
                    }
                }

                int char_x = cursor_x;
                int char_y = cursor_y;
                int pos = 0;

                wmove(typing_win, start_y, 2);
                wattron(typing_win, COLOR_PAIR(1));

                for (int i = 0; i < text_len; i++) {
                    int disp_x = 2 + (i % (max_x - 4));
                    int disp_y = start_y + (i / (max_x - 4));

                    if (i < typed_len) {
                        wattron(typing_win, COLOR_PAIR(2));
                        mvwaddch(typing_win, disp_y, disp_x, display_text[i]);
                        wattroff(typing_win, COLOR_PAIR(2));
                    } else {
                        wattron(typing_win, COLOR_PAIR(1));
                        mvwaddch(typing_win, disp_y, disp_x, display_text[i]);
                        wattroff(typing_win, COLOR_PAIR(1));
                    }
                }
            }
        }
        else if (ch == '\n' || typed_len >= text_len) {
            int correct = 0;
            for (int i = 0; i < typed_len && i < text_len; i++) {
                if (display_text[i] == ch) {
                    correct++;
                }
            }

            float accuracy = typed_len > 0 ? (float)correct / typed_len * 100 : 0;

            mvwprintw(typing_win, max_y - 3, 2, "Game Over! Accuracy: %.1f%%", accuracy);
            mvwprintw(typing_win, max_y - 2, 2, "Press any key to exit...");
            wrefresh(typing_win);
            wgetch(typing_win);
            game_over = 1;
        }
        else if (typed_len < text_len) {
            int correct = (ch == display_text[typed_len]);

            int disp_x = 2 + (typed_len % (max_x - 4));
            int disp_y = start_y + (typed_len / (max_x - 4));

            if (correct) {
                wattron(typing_win, COLOR_PAIR(2)); 
            } else {
                wattron(typing_win, COLOR_PAIR(3));
            }
            mvwaddch(typing_win, disp_y, disp_x, display_text[typed_len]);

            if (correct) {
                wattroff(typing_win, COLOR_PAIR(2));
            } else {
                wattroff(typing_win, COLOR_PAIR(3));
            }

            typed_len++;

            cursor_x++;
            if (cursor_x >= max_x - 2) {
                cursor_x = 2;
                cursor_y++;
            }

            if (typed_len < text_len) {
                int next_x = 2 + (typed_len % (max_x - 4));
                int next_y = start_y + (typed_len / (max_x - 4));

                wattron(typing_win, COLOR_PAIR(4));
                mvwaddch(typing_win, next_y, next_x, display_text[typed_len]);
                wattroff(typing_win, COLOR_PAIR(4));
            }
        }

        wmove(typing_win, cursor_y, cursor_x);
        wrefresh(typing_win);
    }

    delwin(typing_win);
    endwin();
    freeWordList(&wordList);

    return 0;
}
