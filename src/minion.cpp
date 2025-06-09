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
	raw.c_lflag &= ~(ECHO | ICANON);

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
	{}

	return 0;
}

