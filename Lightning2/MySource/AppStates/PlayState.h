#pragma once

#include "BaseState.h"

#include "DefaultParameters.h"
#include "PipelineMgr.h"
#include "Shaders/Lightshader.h"
#include "Light.h"
#include "PlaneMesh.h"

class PlayState : public BaseState
{
public:
	PlayState(D3D* _renderer, HWND _hwnd, int _screenWidth, int _screenHeight, Input* _input, FPCamera* _camera, TextureManager* _textureMgr);
	~PlayState();

	void Init();
	void Update(float _dt);
	void Render();
	void Gui();

private:
	void HandleInput();
	
	FPCamera* camera;
	TextureManager* textureMgr;

	// Pipeline Manager:
	PipelineMgrDefaultSettings defaultSettings;
	PipelineMgr* pipelineMgr;

	// Plane Mesh:
	LightShader* lightShader;
	Light* light;
	PlaneMesh* planeMesh;
	XMMATRIX planeMatrix;	

	bool debugCsv = false;
	bool animatingNow = true;
	bool zappy = false;
	std::string renderMode = DEFAULT_RENDER_MODE;
};
