#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;

class WaterRippleShader : public BaseShader
{
private:
	struct LightBufferType
	{
		XMFLOAT4 diffuse;
		XMFLOAT3 direction;
		float padding;
	};

	struct TimeBufferType
	{
		float time;
		float amplitude;
		float frequency;
		float speed;

	};

	struct TransparentBufferType
	{
		float blendAmount;
		XMFLOAT3 paddingValue;
	};

public:
	WaterRippleShader(ID3D11Device* device, HWND hwnd);
	~WaterRippleShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture, Light* light, float time, float amplitude, float frequency, float speed, float trans);

private:
	void initShader(const wchar_t* cs, const wchar_t* ps);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11Buffer* lightBuffer;
	ID3D11Buffer* timeBuffer;
	ID3D11Buffer* transBuffer;

	ID3D11Device* deviceValue;
	ID3D11BlendState* blendstate;

};

