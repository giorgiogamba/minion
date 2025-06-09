// Copyright 2025 Giorgio Gamba

// Minion, a c++ text editor

#pragma region Includes

#include <iostream>
#include <termios.h>
#include <unistd.h>

#pragma endregion

#pragma region Definitions

#define ERROR_CODE -1

//Bitwises-AND character k with 00011111 (sets the 3 leftmost bits to 0)
// This represents the ctrl behaviour in terminal
#define CTRL_KEY(k) ((k) & 0x1f)

#pragma endregion

void handleError()
{
	std::cerr << "An error occured during execution\n";
	exit(1);
}

#pragma region Terminal Mode

struct termios default_term_settings;

void disableTerminalRawMode()
{
    const int res = tcsetattr(STDIN_FILENO, TCSAFLUSH, &default_term_settings);

	if (res == ERROR_CODE)
	{
		handleError();
	}
}

void enableTerminalRawMode()
{
	const int res = tcgetattr(STDIN_FILENO, &default_term_settings);
	
	if (res == ERROR_CODE)
	{
		handleError();
	}

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

int main()
{
	// Change terminal configurations
	enableTerminalRawMode();

	// Reads 1 byte and writes it in c until it different from q
	while (true)
	{
		char c = '\0';
		const int readRes = read(STDIN_FILENO, &c, 1);
	
		if (readRes == ERROR_CODE && errno != EAGAIN)
		{
			handleError();
		}

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

		if (c == CTRL_KEY('q'))
		{
			break;
		}
	}

	return 0;
}

