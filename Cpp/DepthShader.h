// Colour shader.h
// Simple shader example.
#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;


class DepthShader : public BaseShader
{
	struct tessBufferType
	{
		XMFLOAT4 tessFactor;
		XMFLOAT2 insideFactor;
		XMFLOAT2 padding;
		XMFLOAT4 camPosition;

	};

public:

	DepthShader(ID3D11Device* device, HWND hwnd);
	~DepthShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, XMFLOAT4 tessFactor, XMFLOAT2 insideFactor, ID3D11ShaderResourceView* texture, XMFLOAT4 CamPosition);

private:
	void initShader(const wchar_t* vs, const wchar_t* ps);
	void initShader(const wchar_t* vs, const wchar_t* hs, const wchar_t* ds, const wchar_t* ps);


private:
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* tressBuffer;

	ID3D11SamplerState* sampleState;

};

