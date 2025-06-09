// Copyright 2025 Giorgio Gamba

// Minion, a c++ text editor

#include <iostream>
#include <termios.h>
#include <unistd.h>

void enableTerminalRawMode()
{
	// Read terminal configurations
	struct termios raw;
	tcgetattr(STDIN_FILENO, &raw);

	// Turn off echo mode (each key is printed on terminal)
	raw.c_lflag &= ~(ECHO);

	// Overwrite terminal configurations
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

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

