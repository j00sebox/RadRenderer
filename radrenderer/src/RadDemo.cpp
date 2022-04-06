#include "RadRenderer.h"

#include "Window.h"

int main()
{
	//RadRenderer demo;
	/*if (demo.ConstructConsole(264, 250, 4, 4))
		demo.Start();*/

	Window* p_window = new Window();

	bool running = true;

	while (running)
	{
		if (!p_window->process_messages())
		{
			running = false;
		}

		Sleep(10);
	}

	delete p_window;

	return 0;
}