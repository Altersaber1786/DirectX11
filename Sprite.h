#ifndef SPRITE_H
#define SPRITE_H

#include "DirectDevice.h"
#include "DDSTextureLoader.h"

class Sprite
{
public:
//Attributes

	XMFLOAT3	position_;
	XMFLOAT3	rotation_;
	XMFLOAT3	scale_;

	Sprite();
	~Sprite();
	struct VertexPos
	{
		XMFLOAT3 pos;
		XMFLOAT2 tex0;
	};
	const XMFLOAT3 up = {0.0f, 1.0f, 0.0f};
	bool		Initialize(XMFLOAT3 Position, XMFLOAT3 Rotation, XMFLOAT3 Scale, DirectDevice *device, LPCWSTR fxFile, const wchar_t* texFile);
	void		SetPosition(XMFLOAT3 position);
	void		SetRotation(XMFLOAT3 rotation);
	void		SetScale(XMFLOAT3 scale);
	bool		LoadGeometry(DirectDevice* device, LPCWSTR fxFile, const wchar_t* texFile);
	void		Update();
	void		Render(DirectDevice* device);
	void		Relase();
	XMMATRIX	GetWorldMatrix();
	bool moving;
private:

	XMMATRIX	viewMatrix_;
	XMMATRIX	projMatrix_;
	XMMATRIX	worldMat_;

//Shaders
	ID3D11PixelShader*			pixelShader_;
	ID3D11VertexShader*			vertexShader_;
	ID3D11InputLayout*			inputLayout_;
	ID3D11ShaderResourceView*	shaderResourceView_;
	ID3D11SamplerState*			samplerState_;
	
//Buffers to store vertices
	ID3D11Buffer*				vertexBuffer_;
	ID3D11Buffer*				indexBuffer_;
//Buffers to store constant matrices
	ID3D11Buffer*				projCB_;
	ID3D11Buffer*				worldCB_;





	
};

#endif