#include<iostream>
#include<array>
#include<Windows.h>
#include<sstream>

template<size_t width, size_t height>
using Map = std::array<std::array<int, height>, width>;

size_t width = 30, height = 25;
Map<40, 30> mineMap, UIMap;
COORD GameCursor;
bool gameOver;
const HANDLE STDhandle = GetStdHandle(STD_OUTPUT_HANDLE);
const std::string title = "Minesweeper";
POINT Cursor;
float CalibrationX, CalibrationY;
bool callCalib, hasSpawnedMines;
int bombNumber, timer;

HWND windowHandle = GetConsoleWindow();;

void RevealTile(int x, int y);
void Calibration();
void SetUp();
void Input();
void Logic();
void Draw();


void RevealTile(int x, int y) 
{
	if (mineMap[x][y] == 1)
	{
		UIMap[x][y] = 9;
		gameOver = true;
		return;
	}

	int bombs=0;

	for (int j = -1; j <= 1; j++) {
		for (int i = -1; i <= 1; i++) {
			if ((i != 0 || j != 0) &&x + i < width && y + j < height && x + i >= 0 && y + j >= 0 && mineMap[x + i][y + j] == 1)
				bombs++;
		}
	}


	UIMap[x][y] = bombs;
}


void Calibration()
{

	bool calibrated = false;

	std::cout << "Place Cursor on the bottom right corner of the map" << std::endl;
	std::cout << "L-Click when you are ready, in order to calibrate" << std::endl;

	while (!calibrated) {
		GetCursorPos(&Cursor);
		ScreenToClient(windowHandle, &Cursor);

		if (GetAsyncKeyState(VK_LBUTTON)) {
			CalibrationX = 1.0f * width / Cursor.x;
			CalibrationY = 1.0f * height / Cursor.y;
			calibrated = true;
		}
	}
	system("cls");
}

void SetUp() 
{
	system("cls");
	srand(time(0));
	CalibrationX=1, CalibrationY=1;
	callCalib = true;
	gameOver = false;
	bombNumber = 0;
	timer = 0;
	callCalib = true;
	hasSpawnedMines = false;

	for (int j = 0;j < height; j++) {
		for (int i = 0; i < width; i++) {
			mineMap[i][j] = 0;
			UIMap[i][j] = -1; // Undiscovered tile
		}
	}

}

void Input() 
{
	GetCursorPos(&Cursor);
	ScreenToClient(windowHandle, &Cursor);
	GameCursor.X = CalibrationX * Cursor.x;
	GameCursor.Y = CalibrationY * Cursor.y;

	//recalibration
	if (GetAsyncKeyState(0x43)) {
		callCalib = true;
	}

	if (timer > 5 &&  GameCursor.X < width && GameCursor.X >= 0 && GameCursor.Y < height && GameCursor.Y >= 0) {
		GetAsyncKeyState(VK_LBUTTON);
		GetAsyncKeyState(VK_RBUTTON);

		
		if (UIMap[GameCursor.X][GameCursor.Y] == -1) {
			if (GetAsyncKeyState(VK_LBUTTON)) {
				timer = 0;
			
				if (!hasSpawnedMines) { //Spawning bombs on first click
					hasSpawnedMines = true;
					for (int j = 0; j < height; j++) {
						for (int i = 0; i < width; i++) {
							if (rand() % 6 == 0 && abs(i- GameCursor.X) + abs(j - GameCursor.Y)>(width+height)/8.f) {
								mineMap[i][j] = 1;
								bombNumber++;
							}
							else
								mineMap[i][j] = 0;
						}
					}
				}

				if (mineMap[GameCursor.X][GameCursor.Y] == 1) {
					UIMap[GameCursor.X][GameCursor.Y] = 9; // Bomb hit
					gameOver = true;
				}
				else {
					RevealTile(GameCursor.X, GameCursor.Y);
				}
			}

			
		}
		else if(UIMap[GameCursor.X][GameCursor.Y]>=1&& UIMap[GameCursor.X][GameCursor.Y]<=7) {
			if (GetAsyncKeyState(VK_LBUTTON)) {
				int flags = 0, discovered=0, flagsW=0, discoveredW=0;

				for (int j = -1; j <= 1; j++) {
					for (int i = -1; i <= 1; i++) {
						if ((i != 0 || j != 0))
							if (GameCursor.X + i < width && GameCursor.Y + j < height && GameCursor.X + i >= 0 && GameCursor.Y + j >= 0)
							{
								if (UIMap[GameCursor.X + i][GameCursor.Y + j] == 10)
									flags++;

								else if (UIMap[GameCursor.X + i][GameCursor.Y + j] != -1)
									discovered++;
							}
							else {
								flagsW++;
								discoveredW++;
							}
								
							
					}
				}

				if (flags == UIMap[GameCursor.X][GameCursor.Y]) {
					//
					for (int j = -1; j <= 1; j++) {
						for (int i = -1; i <= 1; i++) {
							if ((i != 0 || j != 0) && GameCursor.X + i < width && GameCursor.Y + j < height && GameCursor.X + i >= 0 && GameCursor.Y + j >= 0 && UIMap[GameCursor.X + i][GameCursor.Y + j] == -1)
								RevealTile(GameCursor.X + i, GameCursor.Y + j);
						}
					}
				}
				if (discovered +discoveredW >= 8-UIMap[GameCursor.X][GameCursor.Y]) {
					//
					for (int j = -1; j <= 1; j++) {
						for (int i = -1; i <= 1; i++) {
							if ((i != 0 || j != 0) && GameCursor.X + i < width && GameCursor.Y + j < height && GameCursor.X + i >= 0 && GameCursor.Y + j >= 0 && UIMap[GameCursor.X + i][GameCursor.Y + j] == -1)
								UIMap[GameCursor.X + i][GameCursor.Y + j] = 10;
						}
					}
				}
			}
		}


		if (GetAsyncKeyState(VK_XBUTTON2)) {
			timer = 0;
			if (UIMap[GameCursor.X][GameCursor.Y] == -1)
				UIMap[GameCursor.X][GameCursor.Y] = 10;
			else if (UIMap[GameCursor.X][GameCursor.Y] == 10)
				UIMap[GameCursor.X][GameCursor.Y] = -1;
		}
	}
}

void Logic()
{
	COORD cc;
	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {

			if (UIMap[i][j] == 0)
			{

				for (int y = -1; y <= 1; y++) {
					for (int x = -1; x <= 1; x++) {
						if ((x != 0 || y != 0) && x + i < width && y + j < height && x + i >= 0 && y + j >= 0 && UIMap[x + i][y + j] == -1)
						{	
							RevealTile(x + i, y + j);
						}
					}
				}
			}

		}
	}

}

void Draw()
{
	SetConsoleCursorPosition(STDhandle, { 0,0 });
	std::stringstream canvas;

	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			if (UIMap[i][j] == -1) {
				canvas << "\xB0\xB0\xB0";
			}
			else if(UIMap[i][j] <=8) {
				canvas << "[";
				if (UIMap[i][j] == 0)
					canvas << "_";
				else {
					canvas << UIMap[i][j];
				}
				canvas << "]";
			}
			else if (UIMap[i][j] == 9)
			{
				canvas << "[X]";
			}
			else if (UIMap[i][j] == 10)
			{
				canvas << " ! ";
			}
		}
		canvas << '\n';
	}
	if(!callCalib)
		canvas << "Press [C] to recalibrate\n";
	canvas << "Bomb Number: " << bombNumber << "                   \n";
	std::cout << canvas.str();

}



int main() {
	SetUp();

	while (!gameOver) {
		
		timer++;

		Input();
		Logic();
		Draw();
		Sleep(40);
		
		//Is only called once
		if (callCalib) {
			Calibration();
			callCalib = false;
		}
	}

	std::cout << "GAME OVER!" << std::endl;
	Beep(100, 500);
	Sleep(1000);
	std::cout << "Try Again?" << std::endl;
	system("pause");
	main();

	return 0;
}