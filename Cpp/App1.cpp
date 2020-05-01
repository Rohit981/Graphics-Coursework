// Lab1.cpp
// Lab 1 example, simple coloured triangle mesh
#include "App1.h"

App1::App1()
{

}

void App1::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input *in, bool VSYNC, bool FULL_SCREEN)
{
	// Call super/parent init function (required!)
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in, VSYNC, FULL_SCREEN);

	// Initalise scene variables.
	TesselatedPlane = new TesselationPlane(renderer->getDevice(), renderer->getDeviceContext());
	orthoMesh = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth / 2, screenHeight / 2 ,-screenWidth / 2, screenHeight / 2.7);
	orthoMeshGodRay = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth , screenHeight);

	cube = new CubeMesh(renderer->getDevice(), renderer->getDeviceContext());
	Water = new PlaneMesh(renderer->getDevice(), renderer->getDeviceContext());
	sun = new SphereMesh(renderer->getDevice(), renderer->getDeviceContext());
	model = new Model(renderer->getDevice(), renderer->getDeviceContext(), "res/Tree.obj");

	textureMgr->loadTexture(L"heightMapTex", L"res/terrainTex.jpg");
	textureMgr->loadTexture(L"heightMap", L"res/snow map height.png");
	textureMgr->loadTexture(L"heightMap2", L"res/height.png");
	textureMgr->loadTexture(L"cubeTex", L"res/sun.jfif");
	textureMgr->loadTexture(L"Water", L"res/WaterTransparent.png");
	textureMgr->loadTexture(L"Tree", L"res/CartoonTree_txtr.jpg");

	depthShader = new DepthShader(renderer->getDevice(), hwnd);
	Tessellatedshader = new TessellationShader(renderer->getDevice(), hwnd);
	renderTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	godRayTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	textureShader = new TextureShader(renderer->getDevice(), hwnd);
	RippleShader = new WaterRippleShader(renderer->getDevice(), hwnd);
	RayShader = new GodRaysShader(renderer->getDevice(), hwnd);
	

	tessfactorX = 2;
	tessfactorY = 2;
	tessfactorZ = 4;
	tessfactorW = 4;

	insidefactorX = 2;
	insidefactorY = 4;

	int shadowmapWidth = 2024;
	int shadowmapHeight = 2024;
	int sceneWidth = 100;
	int sceneHeight = 100;

	lightPositionX = 34.021f;
	lightPositionY = 39.519f;
	lightPositionZ = 0.08f;

	lightDirectionX = 0.057f;
	lightDirectionY = -1.f;
	lightDirectionZ = -1.f;

	SpotlightPositionX = 25.086f;
	SpotlightPositionY = 16.838f;
	SpotlightPositionZ = 34.708f;

	SpotlightDirectionX = 0.f;
	SpotlightDirectionY = -1.f;
	SpotlightDirectionZ = 0.024f;

	cubePositionX = 0.f;
	cubePositionY = 0.f;
	cubePositionZ = 0.f;

	waterPositionX = 243.902f;
	waterPositionY = 3.252f;
	waterPositionZ = 243.902f;


	amplitude = 0.915f;
	frequency = 0.447f;
	speed = 3.720f;

	waterSize = 0.289f;

	trans = 1.f;
	

	// This is your shadow map
	shadowMap = new ShadowMap(renderer->getDevice(), shadowmapWidth, shadowmapHeight);

	//Direction Light
	light = new Light;
	light->setAmbientColour(0.3f, 0.3f, 0.3f, 1.0f);
	light->setDiffuseColour(1.0f, 1.0f, 1.0f, 1.0f);
	light->setDirection(lightDirectionX, lightDirectionY, lightDirectionZ);
	light->setPosition(lightPositionX, lightPositionY, lightPositionZ);
	light->generateOrthoMatrix((float)sceneWidth, (float)sceneHeight, 0.1f, 100.f);

	//Spot Light
	Spotlight = new Light;
	Spotlight->setDiffuseColour(1.0f, 0.0f, 0.0f, 0.0f);
	Spotlight->setAmbientColour(0.3f, 0.3f, 0.3f, 0.3f);
	Spotlight->setPosition(SpotlightPositionX, SpotlightPositionY, SpotlightPositionZ);
	Spotlight->setDirection(SpotlightDirectionX, SpotlightDirectionY, SpotlightDirectionZ);



	//camera->setPosition(lightPositionX, lightPositionY, lightPositionZ);
}


App1::~App1()
{
	// Run base application deconstructor
	BaseApplication::~BaseApplication();

	// Release the Direct3D object.
	
}


bool App1::frame()
{
	bool result;

	result = BaseApplication::frame();
	if (!result)
	{
		return false;
	}


	light->setPosition(lightPositionX, lightPositionY, lightPositionZ);
	light->setDirection(lightDirectionX, lightDirectionY, lightDirectionZ);

	Spotlight->setPosition(SpotlightPositionX, SpotlightPositionY, SpotlightPositionZ);
	Spotlight->setDirection(SpotlightDirectionX, SpotlightDirectionY, SpotlightDirectionZ);

	XMMatrixTranslation(waterPositionX, waterPositionY, waterPositionZ);
	XMMatrixTranslation(modelPositionX, modelPositionY, modelPositionZ);
	XMMatrixScaling(waterSize, waterSize, waterSize);

	timeValue += timer->getTime();
	// Render the graphics.
	result = render();
	if (!result)
	{
		return false;
	}
	



	return true;
}

bool App1::render()
{
	renderPass();
	depthPass();
	GodRaysPass();
	finalPass();

	return true;
}

void App1::renderPass()
{
	renderTexture->setRenderTarget(renderer->getDeviceContext());
	renderTexture->clearRenderTarget(renderer->getDeviceContext(), 0.39f, 0.58f, 0.92f, 1.0f);

	// Get matrices
	camera->update();
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();

	// Render shape with simple lighting shader set.
	TesselatedPlane->sendData(renderer->getDeviceContext());
	Tessellatedshader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"heightMapTex"), textureMgr->getTexture(L"heightMap2"), XMFLOAT4(tessfactorX, tessfactorY, tessfactorZ, tessfactorW),
		XMFLOAT2(insidefactorX, insidefactorY), time, 2, 2, 2, XMFLOAT4(camera->getPosition().x, camera->getPosition().y, camera->getPosition().z, 1), light, shadowMap->getDepthMapSRV(),Spotlight);

	Tessellatedshader->render(renderer->getDeviceContext(), TesselatedPlane->getIndexCount());


	Water->sendData(renderer->getDeviceContext());
	RippleShader->setShaderParameters(renderer->getDeviceContext(), XMMatrixTranslation(waterPositionX, waterPositionY, waterPositionZ) * XMMatrixScaling(waterSize, waterSize, waterSize), viewMatrix, projectionMatrix,
		textureMgr->getTexture(L"Water"), light, timeValue, amplitude, frequency, speed, trans);

	RippleShader->render(renderer->getDeviceContext(), Water->getIndexCount());

	sun->sendData(renderer->getDeviceContext());
	textureShader->setShaderParameters(renderer->getDeviceContext(),   XMMatrixScaling(7.045f, 7.045f, 7.045f) * XMMatrixTranslation(light->getPosition().x, light->getPosition().y, light->getPosition().z), viewMatrix, projectionMatrix, textureMgr->getTexture(L"cubeTex"), light);
	textureShader->render(renderer->getDeviceContext(), sun->getIndexCount());

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	renderer->setBackBufferRenderTarget();
}

void App1::depthPass()
{
	// Set the render target to be the render to texture.
	shadowMap->BindDsvAndSetNullRenderTarget(renderer->getDeviceContext());

	// get the world, view, and projection matrices from the camera and d3d objects.
	light->generateViewMatrix();
	XMMATRIX lightViewMatrix = light->getViewMatrix();
	XMMATRIX lightProjectionMatrix = light->getOrthoMatrix();
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	

	TesselatedPlane->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix, XMFLOAT4(tessfactorX, tessfactorY, tessfactorZ, tessfactorW),
									 XMFLOAT2(insidefactorX, insidefactorY), textureMgr->getTexture(L"heightMap2"), XMFLOAT4(camera->getPosition().x, camera->getPosition().y, camera->getPosition().z, 1));
	depthShader->render(renderer->getDeviceContext(), TesselatedPlane->getIndexCount());

	
	//cube->sendData(renderer->getDeviceContext());
	

	// Set back buffer as render target and reset view port.
	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();
}

void App1::GodRaysPass()
{
	godRayTexture->setRenderTarget(renderer->getDeviceContext());
	godRayTexture->clearRenderTarget(renderer->getDeviceContext(), 0.0f, 0.0f, 0.f, 1.0f);

	// Get matrices
	camera->update();
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();

	// Render shape with simple lighting shader set.
	TesselatedPlane->sendData(renderer->getDeviceContext());
	Tessellatedshader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, NULL, NULL, XMFLOAT4(tessfactorX, tessfactorY, tessfactorZ, tessfactorW),
		XMFLOAT2(insidefactorX, insidefactorY), time, 2, 2, 2, XMFLOAT4(camera->getPosition().x, camera->getPosition().y, camera->getPosition().z, 1), light, shadowMap->getDepthMapSRV(), Spotlight);

	Tessellatedshader->render(renderer->getDeviceContext(), TesselatedPlane->getIndexCount());


	Water->sendData(renderer->getDeviceContext());
	RippleShader->setShaderParameters(renderer->getDeviceContext(), XMMatrixTranslation(waterPositionX, waterPositionY, waterPositionZ) * XMMatrixScaling(waterSize, waterSize, waterSize), viewMatrix, projectionMatrix,
		NULL, light, timeValue, amplitude, frequency, speed, trans);

	RippleShader->render(renderer->getDeviceContext(), Water->getIndexCount());

	sun->sendData(renderer->getDeviceContext());
	textureShader->setShaderParameters(renderer->getDeviceContext(), XMMatrixScaling(7.045f, 7.045f, 7.045f)* XMMatrixTranslation(light->getPosition().x, light->getPosition().y, light->getPosition().z)  , viewMatrix, projectionMatrix, textureMgr->getTexture(L"cubeTex"), light);
	textureShader->render(renderer->getDeviceContext(), sun->getIndexCount());

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	renderer->setBackBufferRenderTarget();
}

void App1::finalPass()
{
	// Clear the scene. (default blue colour)
	renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);
	camera->update();

	// get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	XMMATRIX worldMatrix = renderer->getWorldMatrix();	
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();

	
	
	
	// Render shape with simple lighting shader set.
	TesselatedPlane->sendData(renderer->getDeviceContext());
	Tessellatedshader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"heightMapTex"), textureMgr->getTexture(L"heightMap2"), XMFLOAT4(tessfactorX, tessfactorY, tessfactorZ, tessfactorW),
		XMFLOAT2(insidefactorX, insidefactorY), time, 2, 2, 2, XMFLOAT4(camera->getPosition().x, camera->getPosition().y, camera->getPosition().z, 1), light, shadowMap->getDepthMapSRV(), Spotlight);

	Tessellatedshader->render(renderer->getDeviceContext(), TesselatedPlane->getIndexCount());


	//Render Ortho Mesh
	renderer->setZBuffer(false);

	XMMATRIX orthoMatrix = renderer->getOrthoMatrix();  // ortho matrix for 2D rendering
	XMMATRIX orthoViewMatrix = camera->getOrthoViewMatrix();	// Default camera position for orthographic rendering
	/*XMMATRIX viewMatrixGod = camera->getViewMatrix();
	XMMATRIX projectionMatrixGod = renderer->getProjectionMatrix();*/

	/*orthoMesh->sendData(renderer->getDeviceContext());

	textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, godRayTexture->getShaderResourceView(),light);
	textureShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());
*/
	orthoMeshGodRay->sendData(renderer->getDeviceContext());

	RayShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, godRayTexture->getShaderResourceView(), renderTexture->getShaderResourceView(), light, XMFLOAT2(1024, 1024), viewMatrix, projectionMatrix);
	RayShader->render(renderer->getDeviceContext(), orthoMeshGodRay->getIndexCount());

	renderer->setZBuffer(true);

	gui();
	renderer->endScene();
}

void App1::gui()
{
	// Force turn off unnecessary shader stages.
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->HSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->DSSetShader(NULL, NULL, 0);

	// Build UI
	ImGui::Text("FPS: %.2f", timer->getFPS());
	ImGui::Checkbox("Wireframe mode", &wireframeToggle);
	ImGui::SliderFloat("TessFactorX", &tessfactorX, 1, 64);
	ImGui::SliderFloat("TessFactorY", &tessfactorY, 1, 64);
	ImGui::SliderFloat("TessFactorZ", &tessfactorZ, 1, 64);
	ImGui::SliderFloat("TessFactorW", &tessfactorW, 1, 64);

	ImGui::SliderFloat("LightPositionX", &lightPositionX, -100, 100);
	ImGui::SliderFloat("LightPositionY", &lightPositionY, -100, 100);
	ImGui::SliderFloat("LightPositionZ", &lightPositionZ, -100, 100);

	ImGui::SliderFloat("LightDirectionX", &lightDirectionX, -1, 1);
	ImGui::SliderFloat("LightDirectionY", &lightDirectionY, -1, 1);
	ImGui::SliderFloat("LightDirectionZ", &lightDirectionZ, -1, 1);

	ImGui::SliderFloat("SpotLightDirectionX", &SpotlightDirectionX, -1, 1);
	ImGui::SliderFloat("SpotLightDirectionY", &SpotlightDirectionY, -1, 1);
	ImGui::SliderFloat("SpotLightDirectionZ", &SpotlightDirectionZ, -1, 1);

	ImGui::SliderFloat("SpotLightPositionX", &SpotlightPositionX, -100, 100);
	ImGui::SliderFloat("SpotLightPositionY", &SpotlightPositionY, -100, 100);
	ImGui::SliderFloat("SpotLightPositionZ", &SpotlightPositionZ, -100, 100);

	ImGui::SliderFloat("WaterPositionX", &waterPositionX, -1000, 1000);
	ImGui::SliderFloat("WaterPositionY", &waterPositionY, -1000, 1000);
	ImGui::SliderFloat("WaterPositioZ", &waterPositionZ, -1000, 1000);

	ImGui::SliderFloat("Water Amplitude", &amplitude, 0, 5);
	ImGui::SliderFloat("Water Frequency", &frequency, 0, 5);
	ImGui::SliderFloat("Water Speed", &speed, 0, 5);

	ImGui::SliderFloat("Water Size", &waterSize, 0, 1);

	ImGui::SliderFloat("Ray Size", &raySize, 0, 50);

	ImGui::SliderFloat("Alpha Value", &trans, 0, 1);

	ImGui::SliderFloat("insideFactorX", &insidefactorX, 1, 64);
	ImGui::SliderFloat("insideFactorY", &insidefactorY, 1, 64);
	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

