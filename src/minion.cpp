// Copyright 2025 Giorgio Gamba

// Minion, a c++ text editor

#include <iostream>
#include <termios.h>
#include <unistd.h>

#pragma region Terminal Mode

struct termios default_term_settings;

void disableTerminalRawMode()
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &default_term_settings);
}

void enableTerminalRawMode()
{
	tcgetattr(STDIN_FILENO, &default_term_settings);
	atexit(disableTerminalRawMode);

	// Read terminal configurations and stores them apart
	struct termios raw = default_term_settings;
	tcgetattr(STDIN_FILENO, &raw);

	// Turn off echo mode (each key is printed on terminal)
	// Turn off canonical mode in order to read input byte-by-byte
	// Turn off signals (ctrl+C)
	// Turn off software flow controls
	// Turn off output postprocessing (the one that converts escapes in actions/other escapes)
	raw.c_lflag &= ~(ECHO | ICANON | ISIG);
	raw.c_lflag &= ~(IEXTEN | IXON | ICRNL | OPOST);

	// Turn off other misc flags
	raw.c_lflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON | CS8);

	// Overwrite terminal configurations
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

#pragma endregion

int main()
{
	// Change terminal configurations
	enableTerminalRawMode();

	// Reads 1 byte and writes it in c until it different from q
	char c;
	while (read(STDIN_FILENO, &c, 1) == 1 && c != 'q')
	{
		// Prints only chars from 32 to 126
		// Note that all escape sequences start with byte 27
		if (iscntrl(c))
		{
			printf("%d\r\n", c);
		}
		else
		{
			printf("%d ('%c')\r\n", c, c);	
		}
	}

	return 0;
}

