#include "EmazingEngine.h"

int main()
{
	EmazingEngine game;
	if (game.ConstructConsole(264, 250, 4, 4))
		game.Start();

	return 0;
}