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

    // Initialize color pairs
    init_pair(1, COLOR_WHITE, COLOR_BLACK);   // Untyped text
    init_pair(2, COLOR_GREEN, COLOR_BLACK);   // Correctly typed
    init_pair(3, COLOR_RED, COLOR_BLACK);     // Incorrectly typed
    init_pair(4, COLOR_YELLOW, COLOR_BLACK);  // Current position

    // Word list initialization
    WordList wordList;
    initWordList(&wordList, MAX_WORDS);

    // Read words from file
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

    // Get screen dimensions
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    // Create window
    WINDOW *typing_win = newwin(max_y, max_x, 0, 0);
    keypad(typing_win, TRUE);

    // Display game title and instructions
    mvwprintw(typing_win, 1, max_x/2 - 10, "RIPPLE TYPING GAME");
    mvwprintw(typing_win, 2, 2, "Type the words as they appear. Press ESC to quit.");

    // Seed random number generator
    srand(time(NULL));

    // Select words to display
    int display_count = wordList.count < MAX_DISPLAY_WORDS ? wordList.count : MAX_DISPLAY_WORDS;
    char display_text[MAX_WORD_LEN * MAX_DISPLAY_WORDS] = {0};
    int display_indices[MAX_DISPLAY_WORDS];
    for (int i = 0; i < display_count; i++) {
        display_indices[i] = rand() % wordList.count;
        strcat(display_text, wordList.words[display_indices[i]]);
        strcat(display_text, " ");
    }
    display_text[strlen(display_text)-1] = '\0'; 

    // Calculate text display position
    int start_y = max_y / 2;
    int text_len = strlen(display_text);
    int line_width = max_x - 4;
    int lines_needed = (text_len + line_width - 1) / line_width;
    if (start_y + lines_needed + 5 > max_y) {
        start_y = max_y - lines_needed - 5;
    }

    // Display text in white
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

    // Typing tracking variables
    int typed_len = 0;
    int cursor_x = 2;
    int cursor_y = start_y;
    
    // Array to track character correctness
    char *correctness = malloc(text_len * sizeof(char));
    memset(correctness, 0, text_len * sizeof(char)); // 0 = untyped, 1 = correct, 2 = incorrect

    // Timer and WPM calculation variables
    time_t start_time, end_time;
    int words_typed = 0;
    int correct_chars = 0;

    wmove(typing_win, cursor_y, cursor_x);
    wrefresh(typing_win);

    // Start the timer when the first key is pressed
    int first_keypress = 1;
    int ch;
    int game_over = 0;

    while (!game_over && (ch = wgetch(typing_win)) != 27) { 
        // Start timer on first keypress
        if (first_keypress) {
            start_time = time(NULL);
            first_keypress = 0;
        }

        if (ch == KEY_BACKSPACE || ch == 127) {
            if (typed_len > 0) {
                typed_len--;
                correctness[typed_len] = 0; // Reset correctness for this position
                cursor_x--;
                if (cursor_x < 2) {
                    if (cursor_y > start_y) {
                        cursor_y--;
                        cursor_x = max_x - 2;
                    } else {
                        cursor_x = 2;
                    }
                }
                // Redraw the text with updated colors
                for (int i = 0; i < text_len; i++) {
                    int disp_x = 2 + (i % (max_x - 4));
                    int disp_y = start_y + (i / (max_x - 4));
                    if (i < typed_len) {
                        if (correctness[i] == 1) {
                            wattron(typing_win, COLOR_PAIR(2)); // Correct
                        } else if (correctness[i] == 2) {
                            wattron(typing_win, COLOR_PAIR(3)); // Incorrect
                        }
                        mvwaddch(typing_win, disp_y, disp_x, display_text[i]);
                        wattroff(typing_win, COLOR_PAIR(2));
                        wattroff(typing_win, COLOR_PAIR(3));
                    } else if (i == typed_len) {
                        wattron(typing_win, COLOR_PAIR(4)); // Current position
                        mvwaddch(typing_win, disp_y, disp_x, display_text[i]);
                        wattroff(typing_win, COLOR_PAIR(4));
                    } else {
                        wattron(typing_win, COLOR_PAIR(1)); // Untyped
                        mvwaddch(typing_win, disp_y, disp_x, display_text[i]);
                        wattroff(typing_win, COLOR_PAIR(1));
                    }
                }
            }
        }
        else if (ch == '\n' || typed_len >= text_len) {
            // Calculate end time and elapsed time
            end_time = time(NULL);
            double elapsed_time = difftime(end_time, start_time);

            // Count correct characters and words
            int correct = 0;
            words_typed = 1; // Start with 1 to count the initial word
            for (int i = 0; i < typed_len; i++) {
                if (correctness[i] == 1) {
                    correct++;
                }
                // Count words by spaces
                if (display_text[i] == ' ') {
                    words_typed++;
                }
            }

            // Calculate WPM and accuracy
            float accuracy = typed_len > 0 ? (float)correct / typed_len * 100 : 0;
            float wpm = elapsed_time > 0 ? (words_typed * 60.0) / elapsed_time : 0;

            // Display game over screen
            mvwprintw(typing_win, max_y - 4, 2, "Time: %.0f seconds", elapsed_time);
            mvwprintw(typing_win, max_y - 3, 2, "Words Per Minute (WPM): %.1f", wpm);
            mvwprintw(typing_win, max_y - 2, 2, "Accuracy: %.1f%%", accuracy);
            mvwprintw(typing_win, max_y - 1, 2, "Press any key to exit...");
            wrefresh(typing_win);
            wgetch(typing_win);
            game_over = 1;
        }
        else if (typed_len < text_len) {
            int correct = (ch == display_text[typed_len]);
            correctness[typed_len] = correct ? 1 : 2; // Store if this character was typed correctly
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

    // Cleanup
    free(correctness);
    delwin(typing_win);
    endwin();
    freeWordList(&wordList);
    return 0;
}
