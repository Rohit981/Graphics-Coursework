#include "TessellationShader.h"

TessellationShader::TessellationShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"tessellation_vs.cso", L"tessellation_hs.cso", L"tessellation_ds.cso", L"tessellation_ps.cso");

}

TessellationShader::~TessellationShader()
{
	if (sampleState)
	{
		sampleState->Release();
		sampleState = 0;
	}
	if (matrixBuffer)
	{
		matrixBuffer->Release();
		matrixBuffer = 0;
	}
	if (tressBuffer)
	{
		tressBuffer->Release();
		tressBuffer = 0;
	}
	if (timeBuffer)
	{
		timeBuffer->Release();
		timeBuffer = 0;
	}

	if (layout)
	{
		layout->Release();
		layout = 0;
	}

	//Release base shader components
	BaseShader::~BaseShader();
}

void TessellationShader::initShader(const wchar_t* vsFilename, const wchar_t* hsFilename, const wchar_t* dsFilename, const wchar_t* psFilename)
{
	// InitShader must be overwritten and it will load both vertex and pixel shaders + setup buffers
	initShader(vsFilename, psFilename);

	// Load other required shaders.
	loadHullShader(hsFilename);
	loadDomainShader(dsFilename);
}



void TessellationShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{

	// Load (+ compile) shader files
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	renderer->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	D3D11_BUFFER_DESC tessBufferDesc;
	tessBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	tessBufferDesc.ByteWidth = sizeof(tessBufferType);
	tessBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	tessBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	tessBufferDesc.MiscFlags = 0;
	tessBufferDesc.StructureByteStride = 0;

	renderer->CreateBuffer(&tessBufferDesc, NULL, &tressBuffer);

	D3D11_BUFFER_DESC lightBufferDesc;
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(tessBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	renderer->CreateBuffer(&lightBufferDesc, NULL, &lightBuffer);

	D3D11_BUFFER_DESC SpotlightBufferDesc;
	SpotlightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	SpotlightBufferDesc.ByteWidth = sizeof(SpotLightBufferType);
	SpotlightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	SpotlightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	SpotlightBufferDesc.MiscFlags = 0;
	SpotlightBufferDesc.StructureByteStride = 0;

	renderer->CreateBuffer(&SpotlightBufferDesc, NULL, &spotLightBuffer);


	D3D11_SAMPLER_DESC samplerDesc;
	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	renderer->CreateSamplerState(&samplerDesc, &sampleState);

	// Sampler for shadow map sampling.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.BorderColor[0] = 1.0f;
	samplerDesc.BorderColor[1] = 1.0f;
	samplerDesc.BorderColor[2] = 1.0f;
	samplerDesc.BorderColor[3] = 1.0f;
	renderer->CreateSamplerState(&samplerDesc, &sampleStateShadow);



	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	D3D11_BUFFER_DESC timeBufferDesc;
	timeBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	timeBufferDesc.ByteWidth = sizeof(timeBufferType);
	timeBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	timeBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	timeBufferDesc.MiscFlags = 0;
	timeBufferDesc.StructureByteStride = 0;

	renderer->CreateBuffer(&timeBufferDesc, NULL, &timeBuffer);
}



void TessellationShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* texture1, XMFLOAT4 tessFactor, XMFLOAT2 insideFactor, float time, float amplitude, float frequency, float speed, XMFLOAT4 CamPosition, Light* light, ID3D11ShaderResourceView* depthMap, Light* light2)
{
	HRESULT result;


	D3D11_MAPPED_SUBRESOURCE mappedResource;

	// Transpose the matrices to prepare them for the shader.
	XMMATRIX tworld = XMMatrixTranspose(worldMatrix);
	XMMATRIX tview = XMMatrixTranspose(viewMatrix);
	XMMATRIX tproj = XMMatrixTranspose(projectionMatrix);
	XMMATRIX tLightViewMatrix = XMMatrixTranspose(light->getViewMatrix());
	XMMATRIX tLightProjectionMatrix = XMMatrixTranspose(light->getOrthoMatrix());

	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	MatrixBufferType* dataPtr = (MatrixBufferType*)mappedResource.pData;

	dataPtr->world = tworld;// worldMatrix;
	dataPtr->view = tview;
	dataPtr->projection = tproj;
	dataPtr->lightView = tLightViewMatrix;
	dataPtr->lightProjection = tLightProjectionMatrix;
	deviceContext->Unmap(matrixBuffer, 0);
	deviceContext->DSSetConstantBuffers(0, 1, &matrixBuffer);

	result = deviceContext->Map(tressBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	tessBufferType* tressPtr = (tessBufferType*)mappedResource.pData;

	tressPtr->tessFactor = tessFactor;
	tressPtr->insideFactor = insideFactor;
	tressPtr->camPosition = CamPosition;

	deviceContext->Unmap(tressBuffer, 0);
	deviceContext->HSSetConstantBuffers(0, 1, &tressBuffer);


	deviceContext->PSSetShaderResources(0, 1, &texture);
	deviceContext->DSSetSamplers(0, 1, &sampleState);

	deviceContext->DSSetShaderResources(0, 1, &texture1);
	deviceContext->PSSetSamplers(0, 1, &sampleState);

	LightBufferType* lightPtr;
	deviceContext->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	lightPtr = (LightBufferType*)mappedResource.pData;
	lightPtr->ambient = light->getAmbientColour();
	lightPtr->diffuse = light->getDiffuseColour();
	lightPtr->direction = XMFLOAT4(light->getDirection().x, light->getDirection().y, light->getDirection().z, 1);
	deviceContext->Unmap(lightBuffer, 0);
	deviceContext->PSSetConstantBuffers(0, 1, &lightBuffer);

	SpotLightBufferType* spotLightPtr;
	deviceContext->Map(spotLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	spotLightPtr = (SpotLightBufferType*)mappedResource.pData;

	spotLightPtr->diffuseColour = light2->getDiffuseColour();
	spotLightPtr->ambientColor = light2->getAmbientColour();
	spotLightPtr->position = light2->getPosition();
	spotLightPtr->specularPower = light2->getSpecularPower();
	spotLightPtr->direction = light2->getDirection();
	spotLightPtr->angle = 3.14159f / 16.f;
	spotLightPtr->specularColour = XMFLOAT4(light2->getSpecularColour().x, light2->getSpecularColour().y, light2->getSpecularColour().z, 1);

	deviceContext->Unmap(spotLightBuffer, 0);
	deviceContext->PSSetConstantBuffers(1, 1, &spotLightBuffer);

	result = deviceContext->Map(timeBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	timeBufferType* timePtr = (timeBufferType*)mappedResource.pData;
	timePtr->time = time;
	timePtr->amplitude = amplitude;
	timePtr->frequency = frequency;
	timePtr->speed = speed;

	deviceContext->Unmap(timeBuffer, 0);
	deviceContext->DSSetConstantBuffers(1, 1, &timeBuffer);
	deviceContext->PSSetShaderResources(1, 1, &depthMap);
	deviceContext->PSSetSamplers(0, 1, &sampleState);
	deviceContext->PSSetSamplers(1, 1, &sampleStateShadow);

}