# Ripple Typing Game

Ripple is a terminal-based typing game built with ncurses in C. The game displays random words from a word list and allows users to practice their typing speed and accuracy directly in the terminal.

## Features

- Terminal-based interface using ncurses
- Random word selection from customizable word list
- Color-coded feedback:
  - White: Words to type
  - Green: Correctly typed characters
  - Red: Incorrectly typed characters
  - Yellow: Current character position
- Word wrapping based on terminal width
- Typing accuracy calculation
- Simple, distraction-free interface

## How It Works

The game reads a list of words from a text file and randomly selects words to display. As you type, the characters change color based on whether you typed them correctly or not:

1. Words are displayed in white
2. Correctly typed characters turn green
3. Incorrectly typed characters turn red
4. The current character position is highlighted in yellow

The game tracks your accuracy and displays it at the end of each session.

## Why "Ripple"?

The name "Ripple" was chosen to evoke the fluid, rhythmic nature of typing. Just as a single drop creates ripples that flow outward across water, each keystroke flows into the next when typing smoothly. The name reflects the game's focus on creating a flowing, natural typing experience where one character leads into the next. The visual feedback system, with colors changing as you type, also creates a ripple-like effect of transformation across the screen, reinforcing the connection between the name and the experience of using the application.

## Requirements

- GCC compiler
- ncurses development library

## Installation

### Clone the repository
```bash
git clone https://github.com/SoraFujin/Ripple.git
cd Ripple
```

### Install ncurses (if not already installed)
On Debian/Ubuntu:
```bash
sudo apt-get install libncurses5-dev libncursesw5-dev
```

On Fedora:
```bash
sudo dnf install ncurses-devel
```

On macOS (using Homebrew):
```bash
brew install ncurses
```

### Compile the game
```bash
gcc -o ripple main.c -lncurses
```

## Usage

1. Create a words.txt file in the same directory as the executable, or use the provided sample
2. Run the game:
```bash
./ripple
```
3. Type the words as they appear on the screen
4. Press ESC to quit at any time
5. Your accuracy will be displayed when you complete all the words or press Enter

## Creating Your Own Word List

The game reads words from a file named `words.txt` in the same directory as the executable. Each word should be on a separate line. For example:
```
apple
banana
computer
programming
keyboard
```

## Development Process

The development of Ripple followed these key stages:

1. **Initial Setup**: Created the basic terminal environment using ncurses
2. **Word Loading**: Implemented word loading from file with proper memory management
3. **Display System**: Created a system to display words with proper wrapping
4. **Input Handling**: Implemented keyboard input with special key handling (backspace, enter)
5. **Visual Feedback**: Added color coding to provide immediate visual feedback
6. **Accuracy Tracking**: Implemented tracking and display of typing accuracy

The game was developed with a focus on memory efficiency and smooth terminal rendering, ensuring it works well even on systems with limited resources.

## Code Structure

The code is organized into these main components:

- **Word Management**: Functions to load, store, and free the word list
- **Display Logic**: Code to handle the display of text and cursor management
- **Input Handling**: Processing keyboard input and updating the display
- **Game Logic**: Managing game state and calculating accuracy

## Future Improvements

- Add support for timed typing tests
- Implement difficulty levels
- Add WPM (words per minute) calculation
- Create a highscore system
- Add more customization options
