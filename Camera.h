#ifndef CAMERA_H
#define CAMERA_H

#include "GameObject.h"
class Camera
{
public:
	typedef enum CAMERA_MODE
	{
		CAM_MODE_FREE		=	0,
		CAM_MODE_FOLLOW		=	1,
		CAM_MODE_ARCBALL	=	2
	}CAMERA_MODE;

	Camera();
	~Camera();
	const XMFLOAT3 up_X = XMFLOAT3(1.0f, 0.0f, 0.0f);
	const XMFLOAT3 up_Y = XMFLOAT3(0.0f, 1.0f, 0.0f);
	const XMFLOAT3 up_Z = XMFLOAT3(0.0f, 0.0f, 1.0f);
	const XMFLOAT3 center = XMFLOAT3(0.0f, 0.0f, 0.0f);
	void Follow(GameObject* target);
	void Update(DirectDevice* device);
	bool Initialize(ID3D11Device* device);
	ID3D11Buffer** GetPosCB();
	XMMATRIX viewMatrix;
	void SetPosition(float x, float y, float z);
	XMFLOAT4* GetPosition();
	CAMERA_MODE cam_mode;
	
private:
	
	XMFLOAT4 position;
	struct tessFactors
	{
		float tessFactor;
		XMFLOAT3 padding;
	}tessFact;
	GameObject*			m_target;

	ID3D11Buffer*	camPosCB_;
};

#endif // !CAMERA_H
