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

	std::cout << "Test\n";
	return 0;
}

