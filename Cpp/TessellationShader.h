#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;


class TessellationShader : public BaseShader
{
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
		XMMATRIX lightView;
		XMMATRIX lightProjection;
	};


	struct tessBufferType
	{
		XMFLOAT4 tessFactor;
		XMFLOAT2 insideFactor;
		XMFLOAT2 padding;
		XMFLOAT4 camPosition;

	};

	struct timeBufferType
	{
		float time;
		float amplitude;
		float frequency;
		float speed;

	};

	struct LightBufferType
	{
		XMFLOAT4 ambient;
		XMFLOAT4 diffuse;
		XMFLOAT4 direction;

	};

	struct SpotLightBufferType
	{
		XMFLOAT4 diffuseColour;
		XMFLOAT4 ambientColor;
		XMFLOAT3 position;
		float specularPower;
		XMFLOAT3 direction;
		float angle;
		XMFLOAT4 specularColour;
		
	};

	

public:

	TessellationShader(ID3D11Device* device, HWND hwnd);
	~TessellationShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, ID3D11ShaderResourceView* texture,  ID3D11ShaderResourceView* texture1,  XMFLOAT4 tessFactor, XMFLOAT2 insideFactor, float time, float amplitude, float frequency, float speed, XMFLOAT4 CamPosition, Light * light, ID3D11ShaderResourceView* depthMap, Light * light2);

private:
	void initShader(const wchar_t* vsFilename, const wchar_t* psFilename);
	void initShader(const wchar_t* vsFilename, const wchar_t* hsFilename, const wchar_t* dsFilename, const wchar_t* psFilename);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* tressBuffer;
	ID3D11Buffer* lightBuffer;
	ID3D11Buffer* spotLightBuffer;
	ID3D11Buffer* timeBuffer;

	ID3D11SamplerState* sampleState;
	ID3D11SamplerState* sampleStateShadow;

};

