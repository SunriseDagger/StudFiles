#pragma once
#pragma region system(pause) Unix-implementation
#ifndef WIN32
#include <termios.h>
#include <stdio.h>
void systempause() {
	struct termios info, info_prev;
	tcgetattr(0, &info);          /* get current terminal attirbutes; 0 is the file descriptor for stdin */
	//memcpy((void*)&info_prev, (void*)&info, (size_t)sizeof(struct termios));
	tcgetattr(0, &info_prev);
	info.c_lflag &= ~ICANON;      /* disable canonical mode */
	info.c_cc[VMIN] = 1;          /* wait until at least one keystroke available */
	info.c_cc[VTIME] = 0;         /* no timeout */
	tcsetattr(0, TCSANOW, &info); /* set immediately */
	printf("Press any key to continue...\n");
	getchar();
	// TODO: ignore left chars in buffer
	tcsetattr(0, TCSANOW, &info_prev); /* set buffered */
}
#else
#include <stdlib.h>
void systempause() {
	system("pause");
}
#endif
#pragma endregion

typedef unsigned int uint;