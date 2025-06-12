// Copyright 2025 Giorgio Gamba

// Minion, a c++ text editor

#pragma region Includes

#include <iostream>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#pragma endregion

#pragma region Definitions

#define ERROR_CODE -1

//Bitwises-AND character k with 00011111 (sets the 3 leftmost bits to 0)
// This represents the ctrl behaviour in terminal
#define CTRL_KEY(k) ((k) & 0x1f)

#pragma endregion

#pragma region Types

struct editorConfiguration
{
	// Cursor coordinates
	int cx;
	int cy;

    int screenRows;
    int screenCols;
    struct termios default_term_settings;
};
struct editorConfiguration E;

enum editorKeys
{
	ARROW_LEFT = 'a',
	ARROW_RIGHT = 'd',
	ARROW_UP = 'w',
	ARROW_DOWN = 's',
};

#pragma endregion

int getWindowSize(int& rows, int& cols)
{
	struct winsize ws;

	if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == ERROR_CODE || ws.ws_col == 0)
	{
		return ERROR_CODE;
	}

	cols = ws.ws_col;
	rows = ws.ws_row;

	return 0; // SUCCESS
}

void drawEditorRows()
{
	// #TODO refactor by using a single write operation that prints the complete string
	for (int y = 0; y < E.screenRows; ++y)
	{
		if (y == E.screenRows / 3)
		{
			std::string welcomeMessage{"Welcome to Minion!"};
			write(STDOUT_FILENO, &welcomeMessage, welcomeMessage.size());
		}

		write(STDOUT_FILENO, "~", 1);

		if (y < E.screenRows-1)
		{
			write(STDOUT_FILENO, "\r\n", 2);
		}
	}
}

void refreshScreen()
{
    // Writes an escape character to the terminal (\x1b) which are always followed by [
    // J clears the entire (2) screen
    write(STDOUT_FILENO, "\x1b[2J", 4);

    // Reposition the cursor to the top left corner
    // H takes as optional arguments the XY coords of the desired cursor position
    write(STDOUT_FILENO, "\x1b[H", 3);

	drawEditorRows();

	std::string cursorTerminalPos = "\x1b[" + std::to_string(E.cy+1) + ";" + std::to_string(E.cx + 1) + "H";
	write(STDOUT_FILENO, &cursorTerminalPos, cursorTerminalPos.size()); 
	
	// Reposition cursor
	write(STDOUT_FILENO, "\x1b[H", 3);
}

void handleError()
{
	refreshScreen();
	std::cerr << "An error occured during execution\n";
	exit(1);
}

#pragma region Cursor Movement

// Moves cursor position depending on the passed input
void moveEditorCursor(const char input)
{
	switch(input)
	{
		case 'a':
			E.cx--;
			break;

		case 'd':
			E.cx++;
			break;

		case 'w':
            E.cy++;
            break;

        case 's':
            E.cy--;
            break;

		default:
			break;
	}
}

#pragma endregion

#pragma region Terminal Mode

void disableTerminalRawMode()
{
    const int res = tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.default_term_settings);

	if (res == ERROR_CODE)
	{
		handleError();
	}
}

char readKey()
{
	int numRead = -1;
	char charRead;
	
	while ((numRead = read(STDIN_FILENO, &charRead, 1)) != 1)
	{
		// Error detection
		if (numRead == ERROR_CODE && errno != EAGAIN)
		{
			handleError();
		}
	}

	return charRead;
}

// Handles the editor operation depending on the character
void processKey()
{
	const char c = readKey();

	switch(c)
	{
		case CTRL_KEY('y'):
			std::cout << "Pressed exit combo\n";

			refreshScreen();

			exit(0);
			break;

		case 'a':
		case 's':
		case 'd':
		case 'w':
			moveEditorCursor(c);

		default:
			std::cout << c << "\n";
			break;
	}
}

void enableTerminalRawMode()
{
	const int res = tcgetattr(STDIN_FILENO, &E.default_term_settings);
		
	if (res == ERROR_CODE)
	{
		handleError();
	}

	atexit(disableTerminalRawMode);

	struct termios raw = E.default_term_settings;

	// Read terminal configurations and stores them apart
	tcgetattr(STDIN_FILENO, &raw);

	// Turn off echo mode (each key is printed on terminal)
	// Turn off canonical mode in order to read input byte-by-byte
	// Turn off signals (ctrl+C)
	// Turn off software flow controls
	// Turn off output postprocessing (the one that converts escapes in actions/other escapes)
	raw.c_lflag &= ~(ECHO | ICANON | ISIG);
	raw.c_lflag &= ~(IEXTEN | IXON | ICRNL | OPOST);

	// Setup read time-out
	raw.c_cc[VMIN] = 0;
	raw.c_cc[VTIME] = 1;

	// Turn off other misc flags
	raw.c_lflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON | CS8);

	// Overwrite terminal configurations
	const int writeRes = tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
	if (writeRes == ERROR_CODE)
	{
		handleError();
	}
}

#pragma endregion

void initEditor()
{
	// Cursor is set to the screen's top left
	E.cx = 0;
	E.cy = 0;

	if (getWindowSize(E.screenRows, E.screenCols) == ERROR_CODE)
	{
		handleError();
	}
}

int main()
{
	// Change terminal configurations
	enableTerminalRawMode();
	initEditor();

	// Reads 1 byte and writes it in c until it different from q
	while (true)
	{
		refreshScreen();
		processKey();
	}

	return 0;
}

