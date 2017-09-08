#ifndef CAMERA_H
#define CAMERA_H

#include "Sprite.h"
class Camera
{
public:
	Camera();
	~Camera();
	const XMFLOAT3 up_X = XMFLOAT3(1.0f, 0.0f, 0.0f);
	const XMFLOAT3 up_Y = XMFLOAT3(0.0f, 1.0f, 0.0f);
	const XMFLOAT3 up_Z = XMFLOAT3(0.0f, 0.0f, 1.0f);
	void Follow(Sprite* target);
	void Update(DirectDevice* device);
	bool Initialize(DirectDevice* device);

private:
	Sprite*			m_target_;
	XMMATRIX		cameraView_;
	XMFLOAT3		position_;
	ID3D11Buffer*	viewCB_;
};

#endif // !CAMERA_H
