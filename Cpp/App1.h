// Application.h
#ifndef _APP1_H
#define _APP1_H

// Includes
#include "../DXFramework/DXF.h"
#include "TessellationShader.h"
#include "TesselationPlane.h"
#include "DepthShader.h"
#include "TextureShader.h"
#include "WaterRippleShader.h"
#include "GodRaysShader.h"

class App1 : public BaseApplication
{
public:

	App1();
	~App1();
	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN);

	bool frame();

protected:
	bool render();
	void renderPass();
	void depthPass();
	void GodRaysPass();
	void finalPass();
	void gui();

private:
	TessellationShader* Tessellatedshader;
	TesselationPlane* TesselatedPlane;
	Light* light;
	Light* Spotlight;
	Light* godRaylight;

	PlaneMesh* Water;
	SphereMesh* sun;
	WaterRippleShader* RippleShader;
	GodRaysShader* RayShader;
	
	DepthShader* depthShader;
	OrthoMesh* orthoMesh;
	OrthoMesh* orthoMeshGodRay;
	CubeMesh* cube;

	RenderTexture* renderTexture;
	RenderTexture* godRayTexture;

	ShadowMap* shadowMap;
	TextureShader* textureShader;
	Model* model;
	

	float lightPositionX;
	float lightPositionY;
	float lightPositionZ;

	float lightDirectionX;
	float lightDirectionY;
	float lightDirectionZ;

	float SpotlightPositionX;
	float SpotlightPositionY;
	float SpotlightPositionZ;

	float SpotlightDirectionX;
	float SpotlightDirectionY;
	float SpotlightDirectionZ;

	float tessfactorX;
	float tessfactorY;
	float tessfactorZ;
	float tessfactorW;

	float insidefactorX;
	float insidefactorY;

	float cubePositionX;
	float cubePositionY;
	float cubePositionZ;

	float cameraPositionX;
	float cameraPositionY;
	float cameraPositionZ;

	float waterPositionX;
	float waterPositionY;
	float waterPositionZ;

	float modelPositionX;
	float modelPositionY;
	float modelPositionZ;

	float waterSize;
	float raySize;

	float amplitude;
	float frequency;
	float speed;

	float trans;

	float time;
	float timeValue;

};

#endif