#include <stdio.h>
#include <windows.h>
#include <time.h>

#define scount 30
#define screen_x 80
#define screen_y 25

HANDLE wHnd;
HANDLE rHnd;
DWORD fdwMode;
CHAR_INFO consoleBuffer[screen_x * screen_y];
COORD bufferSize = { screen_x,screen_y };
COORD characterPos = { 0,0 };
SMALL_RECT windowSize = { 0,0,screen_x - 1,screen_y - 1 };
COORD star[scount];
COORD ship;

int setConsole(int x, int y)
{
	wHnd = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleWindowInfo(wHnd, TRUE, &windowSize);
	SetConsoleScreenBufferSize(wHnd, bufferSize);
	return 0;
}

int setMode()
{
	rHnd = GetStdHandle(STD_INPUT_HANDLE);
	fdwMode = ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT |
		ENABLE_MOUSE_INPUT;
	SetConsoleMode(rHnd, fdwMode);
	return 0;
}

void clear_buffer()
{
	for (int y = 0; y < screen_y; ++y) {
		for (int x = 0; x < screen_x; ++x) {
			consoleBuffer[x + screen_x * y].Char.AsciiChar = ' ';
			consoleBuffer[x + screen_x * y].Attributes = 0;
		}
	}
}

void fill_buffer_to_console()
{
	WriteConsoleOutputA(wHnd, consoleBuffer, bufferSize, characterPos,
		&windowSize);
}

void init_star()
{
	for (int i = 0; i < scount; i++) {
		star[i] = { (rand() % screen_x) , (rand() % screen_y) };
	}

}

void star_fall()
{
	int i;
	for (i = 0; i < scount; i++) {
		if (star[i].Y >= screen_y - 1) {
			star[i] = { (rand() % screen_x) , 1 };
		}
		else {
			star[i] = { star[i].X, star[i].Y + 1 };
		}
	}

}

void fill_star_to_buffer()
{
	for (int j = 0; j < scount; j++) {
		consoleBuffer[star[j].X + screen_x * star[j].Y].Char.AsciiChar = '*';
		consoleBuffer[star[j].X + screen_x * star[j].Y].Attributes = 7;
	}
}

void draw_ship(int posx, int posy, int colorr)
{
	consoleBuffer[(posx - 2) + screen_x * posy].Char.AsciiChar = '<';
	consoleBuffer[(posx - 2) + screen_x * posy].Attributes = colorr;
	consoleBuffer[(posx - 1) + screen_x * posy].Char.AsciiChar = '-';
	consoleBuffer[(posx - 1) + screen_x * posy].Attributes = colorr;
	consoleBuffer[posx + screen_x * posy].Char.AsciiChar = '0';
	consoleBuffer[posx + screen_x * posy].Attributes = colorr;
	consoleBuffer[(posx + 1) + screen_x * posy].Char.AsciiChar = '-';
	consoleBuffer[(posx + 1) + screen_x * posy].Attributes = colorr;
	consoleBuffer[(posx + 2) + screen_x * posy].Char.AsciiChar = '>';
	consoleBuffer[(posx + 2) + screen_x * posy].Attributes = colorr;
}

int main()
{
	int i, num = 0;
	int color = 7;
	bool play = true;
	DWORD numEvents = 0;
	DWORD numEventsRead = 0;
	srand(time(NULL));
	setConsole(screen_x, screen_y);
	init_star();
	setMode();

	while (play)
	{
		star_fall();
		clear_buffer();
		fill_star_to_buffer();
		GetNumberOfConsoleInputEvents(rHnd, &numEvents);
		if (numEvents != 0) {
			INPUT_RECORD* eventBuffer = new INPUT_RECORD[numEvents];
			ReadConsoleInput(rHnd, eventBuffer, numEvents, &numEventsRead);
			for (DWORD i = 0; i < numEventsRead; ++i) {

				if (eventBuffer[i].EventType == KEY_EVENT && eventBuffer[i].Event.KeyEvent.bKeyDown == true) {
					if (eventBuffer[i].Event.KeyEvent.wVirtualKeyCode == VK_ESCAPE) {
						play = false;
					}
					if (eventBuffer[i].Event.KeyEvent.uChar.AsciiChar == 99) {
						color = rand() % 9;
					}
				}

				else if (eventBuffer[i].EventType == MOUSE_EVENT) {
					int posx = eventBuffer[i].Event.MouseEvent.dwMousePosition.X;
					int posy = eventBuffer[i].Event.MouseEvent.dwMousePosition.Y;

					if (eventBuffer[i].Event.MouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) {
						color = rand() % 9;
					}
					else if (eventBuffer[i].Event.MouseEvent.dwEventFlags & MOUSE_MOVED) {
						ship.X = posx;
						ship.Y = posy;
						draw_ship(ship.X, ship.Y, color);
					}
				}

			}
			delete[] eventBuffer;
		}

		for (int k = 0; k < scount; k++)
		{
			if ((star[k].X == ship.X && star[k].Y == ship.Y) ||
				(star[k].X == ship.X - 2 && star[k].Y == ship.Y) ||
				(star[k].X == ship.X - 1 && star[k].Y == ship.Y) ||
				(star[k].X == ship.X + 1 && star[k].Y == ship.Y) ||
				(star[k].X == ship.X + 2 && star[k].Y == ship.Y)) {

				star[k] = { (rand() % screen_x) , (rand() % screen_y) };
				num += 1;
			}
			if (num == 10) { play = false; }
		}

		fill_buffer_to_console();
		Sleep(200);

	}
	return 0;
}