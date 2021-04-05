// Main.cpp

#include "System.h"
#include "App1.h"

#if 0
	const int SCREEN_WIDTH = 1200;
	const int SCREEN_HEIGHT = 675;
#elif 1
	const int SCREEN_WIDTH  = 1536;
	const int SCREEN_HEIGHT = 864;
#elif 0
	const int SCREEN_WIDTH = 1920;
	const int SCREEN_HEIGHT = 1080;
#endif

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	cout << __cplusplus;

	App1* app = new App1();
	System* system;

	// Create the system object.
	system = new System(app, SCREEN_WIDTH, SCREEN_HEIGHT, true, false);
	
	// Initialize and run the system object.
	system->run();

	// Shutdown and release the system object.
	delete system;
	system = 0;

	return 0;
}