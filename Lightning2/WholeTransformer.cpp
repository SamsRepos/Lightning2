#include "WholeTransformer.h"

void WholeTransformer::InitParameters(
	MyFloat3 _startPoint,
	MyFloat3 _endPoint
)
{
	startPoint = _startPoint;
	endPoint   = _endPoint;
}

void WholeTransformer::Run()
{
	// PLAN:
	// 1. transform to origin	
	// 2. align along start->end vector	
	// 3. scale to start->end magnitude
	// 4. transform to start point

	Segment* root = segments->front();

	// 1. transform to origin
	TranslateRecurs(root, MyFloat3(0.f, 0.f, 0.f));

	// 2. align along start->end vector
	MyFloat3 desiredDirection = endPoint - startPoint;
	MyFloat3 currentDirection = GetFarthestEndPointRecurs(root);
	AlignSegments(desiredDirection, currentDirection);

	// 3. scale to start->end magnitude
	// 4. transform to start point
}

struct MyFloat4
{
	float x;
	float y;
	float z;
	float w;

	MyFloat4()
	{
		x = y = z = 0.f;
		w = 1.f;
	}

	MyFloat4(float _x, float _y, float _z, float _w = 1.f)
	{
		x = _x;
		y = _y;
		z = _z;
		w = _w;
	}

	MyFloat4(MyFloat3 float3, float _w = 1.f)
	{
		x = float3.x;
		y = float3.y;
		z = float3.z;
		w = _w;
	}

	float Magnitude();
	
	MyFloat3 Normalised();
};

struct MyMatrix44
{
	MyFloat4 values[4];

	MyMatrix44(const float *m) 
	{
		values[0] = MyFloat4(m[0], m[1], m[2], m[3]);
		values[1] = MyFloat4(m[4], m[5], m[6], m[7]);
		values[2] = MyFloat4(m[8], m[9], m[10], m[11]);
		values[3] = MyFloat4(m[12], m[13], m[14], m[15]);
	}	
};

MyFloat4 operator*(const MyFloat4& vec, const MyMatrix44& mat)
{
	MyFloat4 res;

	res.x = (vec.x * mat.values[0].x) + (vec.y * mat.values[1].x) + (vec.z * mat.values[2].x) + (vec.w * mat.values[3].x);
	res.y = (vec.x * mat.values[0].y) + (vec.y * mat.values[1].y) + (vec.z * mat.values[2].y) + (vec.w * mat.values[3].y);
	res.z = (vec.x * mat.values[0].z) + (vec.y * mat.values[1].z) + (vec.z * mat.values[2].z) + (vec.w * mat.values[3].z);
	res.w = (vec.x * mat.values[0].w) + (vec.y * mat.values[1].w) + (vec.z * mat.values[2].w) + (vec.w * mat.values[3].w);
	
	return res;
}

MyFloat3 operator*(const MyFloat3& vec, const MyMatrix44& mat)
{
	MyFloat4 res = MyFloat4(vec) * mat;
	return MyFloat3(
		res.x / res.w,
		res.y / res.w,
		res.z / res.w
	);
}

void WholeTransformer::TranslateRecurs(Segment* currentSegment, MyFloat3 currentStartPoint)
{
	MyFloat3 direction = currentSegment->GetDirection();
	currentSegment->SetStartPoint(currentStartPoint);
	currentSegment->SetEndPoint(currentStartPoint + direction);

	for (Segment* child : *(currentSegment->GetChildren()))
	{
		TranslateRecurs(child, currentSegment->GetEndPoint());
	}
}

MyFloat3 WholeTransformer::GetFarthestEndPointRecurs(Segment* currentSegment)
{
	if (currentSegment->GetChildren()->size() == 0)
	{
		return currentSegment->GetEndPoint();
	}
	else
	{
		for (Segment* child : *(currentSegment->GetChildren()))
		{
			if (child->GetStatus() == SegmentStatuses::PRIMARY)
			{
				return GetFarthestEndPointRecurs(child);
			}
		}
	}
}

void WholeTransformer::AlignSegments(MyFloat3 desiredDirection, MyFloat3 currentDirection)
{
	MyFloat3 rotationAxis = CrossProduct(desiredDirection, currentDirection).Normalised();

	float alpha = rotationAxis.x;
	float beta  = rotationAxis.y;
	float gamma = rotationAxis.z;

	float alphaSquared = alpha * alpha;
	float betaSquared  = beta * beta;
	float gammaSquared = gamma * gamma;

	float alphaBeta  = alpha * beta;
	float alphaGamma = alpha * gamma;
	float betaGamma  = beta * gamma;

	float cosTheta = DotProduct(desiredDirection.Normalised(), currentDirection.Normalised());
	float theta    = acos(cosTheta);
	float sinTheta = sin(theta);

	float rotationValues[] = {
		(alphaSquared * (1 - cosTheta) + cosTheta),      (alphaBeta * (1 - cosTheta) + gamma * sinTheta), (alphaGamma * (1 - cosTheta) - beta * sinTheta), 0.f,
		(alphaBeta * (1 - cosTheta) - gamma * sinTheta), (betaSquared * (1 - cosTheta) + cosTheta),       (betaGamma * (1 - cosTheta) - alpha * sinTheta), 0.f,
		(alphaGamma * (1 - cosTheta) + beta * sinTheta), (betaGamma * (1 - cosTheta) - alpha * sinTheta), (gammaSquared * (1 - cosTheta) + cosTheta),      0.f,
		0.f,                                             0.f,                                             0.f,                                             1.f
	};	

	MyMatrix44 rotationMatrix(rotationValues);

	for (Segment* segment : *segments)
	{
		segment->SetStartPoint(segment->GetStartPoint() * rotationMatrix);
		segment->SetEndPoint(segment->GetEndPoint() * rotationMatrix);
	}

}