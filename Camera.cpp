#include "Camera.h"



Camera::Camera()
{
	viewMatrix = XMMatrixIdentity();
	position = XMFLOAT4( 0.0f, 0.0f, 0.0f, 0.0f );
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
	constDesc.ByteWidth = sizeof(tessFactors);
	if (FAILED(device->CreateBuffer(&constDesc, 0, &tessFactorsCB)))
	{
		MessageBox(NULL, "Failed to create tessellation factors const buffer", NULL, NULL);
		return false;
	}
	return true;
}

void Camera::Follow(GameObject* target)
{
	m_target = target;
}

void Camera::Update(DirectDevice* device)
{
	if (m_target != nullptr)
	{
		position.x = m_target->position_.x;
		//position.y = m_target->position_.y;
		viewMatrix = XMMatrixLookAtLH(XMLoadFloat4(&position), XMLoadFloat3(&(m_target->position_)), XMLoadFloat3(&up_Y));
	}
	device->d3dContext_->UpdateSubresource(camPosCB_, 0, 0, &position, 0, 0);
	float distance = static_cast<float>(sqrt(pow((position.x - m_target->position_.x), 2) + pow((position.y - m_target->position_.y), 2) + pow((position.z - m_target->position_.z), 2)));
	tessFact.tessFactor = (20.0f / distance);
	device->d3dContext_->UpdateSubresource(tessFactorsCB, 0, 0, &tessFact, 0, 0);
	device->d3dContext_->HSSetConstantBuffers(0, 1, &tessFactorsCB);
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
