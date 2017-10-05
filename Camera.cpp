#include "Camera.h"



Camera::Camera()
{
	viewMatrix = XMMatrixIdentity();
	viewMatrix = XMMatrixTranspose(viewMatrix);
	position = XMFLOAT4( 0.0f, 0.0f, 0.0f, 0.0f );
}

bool Camera::Initialize(ID3D11Device* device)
{
	D3D11_BUFFER_DESC constDesc;
	ZeroMemory(&constDesc, sizeof(constDesc));
	constDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constDesc.ByteWidth = sizeof(viewMatrix);
	constDesc.Usage = D3D11_USAGE_DEFAULT;
	//constant buffer to store view matrix
	
	if (FAILED(device->CreateBuffer(&constDesc, 0, &viewCB_)))
	{
		MessageBox(NULL, "Failed to create camera view const buffer", NULL, NULL);
		return false;
	}
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
	m_target = target;
	viewMatrix = XMMatrixLookAtLH(XMLoadFloat4(&position), XMLoadFloat3(&(target->position_)), XMLoadFloat3(&up_Y));
	viewMatrix = XMMatrixTranspose(viewMatrix);
}

void Camera::Update(DirectDevice* device)
{
	if (m_target != nullptr)
	{
		position.x = m_target->position_.x;
		position.y = m_target->position_.y;
		viewMatrix = XMMatrixLookAtLH(XMLoadFloat4(&position), XMLoadFloat3(&(m_target->position_)), XMLoadFloat3(&up_Y));
		viewMatrix = XMMatrixTranspose(viewMatrix);
	}
	device->d3dContext_->UpdateSubresource(viewCB_, 0, 0, &viewMatrix, 0, 0);
	device->d3dContext_->UpdateSubresource(camPosCB_, 0, 0, &position, 0, 0);
	device->d3dContext_->VSSetConstantBuffers(1, 1, &viewCB_);
}

ID3D11Buffer** Camera::GetPosCB()
{
	return &camPosCB_;
}

Camera::~Camera()
{
	 viewCB_->Release();
	 camPosCB_->Release();
	 viewCB_ = 0;
	 camPosCB_ = 0;
	 m_target = 0;
}
