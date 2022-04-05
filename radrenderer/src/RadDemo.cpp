#include "RadRenderer.h"

int main()
{
	RadRenderer demo;
	if (demo.ConstructConsole(264, 250, 4, 4))
		demo.Start();

	return 0;
}