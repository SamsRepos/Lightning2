#include "LineRenderer.h"

#include "Utils/MyVectorUtil.h"
#include "Maths/MyClamp.h"

////
// PUBLIC:
////

LineRenderer::LineRenderer(D3D* renderer, HWND hwnd)
	:
	shader(NULL),
	mesh(NULL)	
{
	if (shader)
	{
		delete shader;
	}
	shader = new LineShader(renderer->getDevice(), hwnd);

	if (mesh)
	{
		delete mesh;
	}
	mesh = new LineMesh(renderer->getDevice(), renderer->getDeviceContext());
}

LineRenderer::~LineRenderer()
{
	if (shader)
	{
		delete shader;
	}

	if (mesh)
	{
		delete mesh;
	}
}

void LineRenderer::Build(std::vector<Segment*>* segments)
{
	DeleteAllVectorData(&lines);

	Segment* rootSeg = segments->front();

	CreateLinesRecurs(rootSeg, NULL);
	
	if (mesh)
	{
		mesh->SetLines(&lines);
	}
}

void LineRenderer::InitAnimation()
{
	animatingNow = true;

	for (Line* line : lines)
	{
		line->InitAnimation();
	}

	Line* rootLine = lines.front();
	rootLine->SetVisible(true);
}

//returns true when animation is over
bool LineRenderer::UpdateAnimation(float dt)
{
	if (lines.size() > 0 && animatingNow)
	{
		Line* rootLine = lines.front();

		bool isFinished = rootLine->UpdateAnimationRecurs(dt);
		animatingNow = !isFinished;
	}

	return !animatingNow;
}


void LineRenderer::SetShaderParams(
	const XMMATRIX& _worldMatrix,
	const XMMATRIX& _viewMatrix,
	const XMMATRIX& _projectionMatrix
)
{
	worldMatrix      = _worldMatrix;
	viewMatrix       = _viewMatrix;
	projectionMatrix = _projectionMatrix;
}

void LineRenderer::RenderLines(D3D* renderer)
{
	if (shader && mesh)
	{
		int linesToRender = int(mesh->GetLineCount());

		for (int i = 0; i < linesToRender; i++)
		{
			shader->setShaderParameters(
				renderer->getDeviceContext(),
				worldMatrix,
				viewMatrix,
				projectionMatrix,
				NULL,
				lineColour
			);

			mesh->sendData(renderer->getDeviceContext(), i);
			shader->render(renderer->getDeviceContext(), mesh->getIndexCount());
		}
	}
}

////
// PRIVATE:
////

void LineRenderer::CreateLinesRecurs(Segment* seg, Line* parentLine)
{
	Line* newLine = new Line(seg);
	
	//newLine->SetBrightness(
	//	MyClamp(
	//	(seg->GetEnergy() / maxEnergy),
	//		0.f,
	//		1.f
	//	)
	//);

	if (parentLine)
	{
		parentLine->AddChild(newLine);
		newLine->SetParent(parentLine);
	}

	lines.push_back(newLine);

	for (Segment* s : *(seg->GetChildren()))
	{
		CreateLinesRecurs(s, newLine);
	}
}