#include "RadRenderer.h"

#include "Window.h"

int main()
{
	//RadRenderer demo;
	/*if (demo.ConstructConsole(264, 250, 4, 4))
		demo.Start();*/

	Window window(264, 250);

	window.loop();

	return 0;
}