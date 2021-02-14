#include "App1.h"

#include <random>
#include <ctime>

#include "ColourValues.h"

////
// PUBLIC:
////

App1::App1()
	:
	currentState(NULL),
	playState(NULL),
	testState(NULL)
{
}

void App1::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input *in, bool VSYNC, bool FULL_SCREEN)
{
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in, VSYNC, FULL_SCREEN);
	
	srand(time(NULL));

	//TEXTURES:
	textureMgr->loadTexture(L"grass", L"res/grass.jpg");
	textureMgr->loadTexture(L"metal", L"res/metal.jpg");
	
	//Camera:
	camera->setPosition(-20.f, 50.f, -130.f);
	camera->setRotation(0.0f, 0.0f, 0.0f);

	// App States:
	playState = new PlayState(renderer, hwnd, screenWidth, screenHeight, camera, textureMgr);
	playState->Init();
	testState = new TestState(renderer, hwnd, screenWidth, screenHeight);
	testState->Init();

	currentState = playState;
}

App1::~App1()
{
	// Run base application deconstructor
	BaseApplication::~BaseApplication();

	if (playState)
	{
		delete playState;
		playState = NULL;
	}

	if (testState)
	{
		delete testState;
		testState = NULL;
	}
}

bool App1::frame()
{
	bool result;

	result = BaseApplication::frame();
	if (!result)
	{
		return false;
	}
	
	float dt = timer->getTime();

	currentState->Update(dt);
	
	// Render the graphics.
	result = render();
	if (!result)
	{
		return false;
	}

	return true;
}

bool App1::render()
{
	XMFLOAT4 backgroundColour = NIGHT_BACKGROUND_COLOUR;
	renderer->beginScene(backgroundColour.x, backgroundColour.y, backgroundColour.z, backgroundColour.w);

	// Generate the view matrix based on the camera's position.
	camera->update();

	currentState->Render();
		
	// Render GUI
	Gui();

	// Swap the buffers
	renderer->endScene();

	return true;
}

////
// PRIVATE:
////

void App1::Gui()
{
	// Force turn off unnecessary shader stages.
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->HSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->DSSetShader(NULL, NULL, 0);

	// Build UI
	ImGui::Text("FPS: %.2f", timer->getFPS());
	ImGui::Text("Camera Pos: (%.2f, %.2f, %.2f)", camera->getPosition().x, camera->getPosition().y, camera->getPosition().z);
	ImGui::Checkbox("Wireframe mode", &wireframeToggle);

	ImGui::Text("*************************************");

	ImGui::Text("State Switching:");

	if (ImGui::Button("Play State"))
	{
		currentState = playState;
	}
	ImGui::SameLine();
	if (ImGui::Button("Test State"))
	{
		currentState = testState;
	}

	/*if (ImGui::Button("Reset Current State"))
	{
		currentState->Init();
	}*/

	ImGui::Text("*************************************");

	currentState->Gui();


// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}
