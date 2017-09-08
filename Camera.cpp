#include "Camera.h"



Camera::Camera()
{
	cameraView_ = XMMatrixIdentity();
	cameraView_ = XMMatrixTranspose(cameraView_);
	position_ = XMFLOAT3(0.0f, 0.0f, 0.0f);
}

bool Camera::Initialize(DirectDevice* device)
{
	D3D11_BUFFER_DESC constDesc;
	ZeroMemory(&constDesc, sizeof(constDesc));
	constDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constDesc.ByteWidth = sizeof(XMMATRIX);
	constDesc.Usage = D3D11_USAGE_DEFAULT;
	//view matrix
	
	if (FAILED(device->d3dDevice_->CreateBuffer(&constDesc, 0, &viewCB_)))
	{
		return false;
	}
	return true;
}

void Camera::Follow(Sprite* target)
{
	m_target_ = target;
	cameraView_ = XMMatrixLookAtLH(XMLoadFloat3(&position_), XMLoadFloat3(&(target->position_)), XMLoadFloat3(&up_Y));
	cameraView_ = XMMatrixTranspose(cameraView_);
}

void Camera::Update(DirectDevice* device)
{
	position_.x = m_target_->position_.x;
	cameraView_ = XMMatrixLookAtLH(XMLoadFloat3(&position_), XMLoadFloat3(&(m_target_->position_)), XMLoadFloat3(&up_Y));
	cameraView_ = XMMatrixTranspose(cameraView_);
	device->d3dContext_->UpdateSubresource(viewCB_, 0, 0, &cameraView_, 0, 0);
	device->d3dContext_->VSSetConstantBuffers(1, 1, &viewCB_);
}

Camera::~Camera()
{
	if (viewCB_) viewCB_->Release();
}
