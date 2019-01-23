#include "Camera.h"



Camera::Camera()
{
	cam_mode = CAM_MODE_FREE;
	viewMatrix = XMMatrixIdentity();
	position = XMFLOAT4( 0.0f, 0.0f, 700.0f, 0.0f );
	tessFact.tessFactor = 1.0f;
}
void Camera::SetPosition(float x, float y, float z)
{
	position.x = x;
	position.y = y;
	position.z = z;
}
XMFLOAT4* Camera::GetPosition()
{
	return &position;
}
bool Camera::Initialize(ID3D11Device* device)
{
	D3D11_BUFFER_DESC constDesc;
	ZeroMemory(&constDesc, sizeof(constDesc));
	constDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constDesc.Usage = D3D11_USAGE_DEFAULT;
	//constant buffer to store view matrix
	constDesc.ByteWidth = sizeof(XMFLOAT4);
	
	if (FAILED(device->CreateBuffer(&constDesc, 0, &camPosCB_)))
	{
		MessageBox(NULL, "Failed to create camera view const buffer", NULL, NULL);
		return false;
	}

	return true;
}

void Camera::Follow(GameObject* target)
{
	cam_mode = CAM_MODE_FOLLOW;
	m_target = target;
}

void Camera::Update(DirectDevice* device)
{
	switch (cam_mode)
	{
	case CAM_MODE_FREE: break;
	case CAM_MODE_FOLLOW:
	{
		if (m_target != nullptr)
		{
			position.x = m_target->position_.x;
			viewMatrix = XMMatrixLookAtLH(XMLoadFloat4(&position), XMLoadFloat3(&(m_target->position_)), XMLoadFloat3(&up_Y));
		}
		break;
	}
	case CAM_MODE_ARCBALL: {
		viewMatrix = XMMatrixLookAtLH(XMLoadFloat4(&position), XMLoadFloat3(&(center)), XMLoadFloat3(&up_Y));
		break;
	        }
	}
	
	device->d3dContext_->UpdateSubresource(camPosCB_, 0, 0, &position, 0, 0);
	
	device->d3dContext_->HSSetConstantBuffers(0, 1, &camPosCB_);
}

ID3D11Buffer** Camera::GetPosCB()
{
	return &camPosCB_;
}

Camera::~Camera()
{
	 camPosCB_->Release();
	 camPosCB_ = 0;
	 m_target = 0;
}
