#ifndef CAMERA_H
#define CAMERA_H

#include "GameObject.h"
class Camera
{
public:
	Camera();
	~Camera();
	const XMFLOAT3 up_X = XMFLOAT3(1.0f, 0.0f, 0.0f);
	const XMFLOAT3 up_Y = XMFLOAT3(0.0f, 1.0f, 0.0f);
	const XMFLOAT3 up_Z = XMFLOAT3(0.0f, 0.0f, 1.0f);
	void Follow(GameObject* target);
	void Update(DirectDevice* device);
	bool Initialize(ID3D11Device* device);
	ID3D11Buffer** GetPosCB();

private:
	
	XMMATRIX viewMatrix;
	XMFLOAT4 position;
	
	GameObject*			m_target;

	ID3D11Buffer*	viewCB_;
	ID3D11Buffer*	camPosCB_;
};

#endif // !CAMERA_H
