#include <ncurses.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>

#define MAX_WORD_LEN 50
#define INITIAL_WORDS 1024

void init_colors()
{
	if(has_colors() == FALSE)
	{
		endwin();
		printf("Terminal Does Not Support COLORS \n");
		return;
	}

	start_color();
	init_pair(1, COLOR_RED, COLOR_BLACK);
	init_pair(2, COLOR_GREEN, COLOR_BLACK);
}

void read_file()
{
	const char *path = "words.txt";
	FILE *file = fopen(path, "r");

	if(file == NULL)
	{
		perror("Error opening file");
		return;
	}

	char buffer[INITIAL_WORDS];


	while(fgets(buffer, sizeof(buffer), file) != NULL) 
	{
		size_t length = strlen(buffer);
		for(int i = 0; i < length; i++)
		{
			if(buffer[length - 1] == '\n')
			{
				buffer[length - 1] = '\0';
				length--;
				printf("%s ", buffer);
			}
		}
	}

	fclose(file);
}

int main(void) 
{
	initscr();            
	raw();             
	keypad(stdscr, TRUE);
	noecho();           
	curs_set(1);       

	printw("Hello, ncurses!\n");
	int ch = getch();

	if(ch == KEY_F(1))
		printw("F1 is pressed");
	else 
	{
		printw("The printed KEY is\n");
		attron(A_BOLD);
		printw("%c", ch);
		attroff(A_BOLD);
	}
	refresh();	

	getch();         
	endwin();       
	return 0;
}
