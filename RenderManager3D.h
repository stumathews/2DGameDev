#pragma once

#include "COMUtils.h"
#include "Sprites3D.h"
#include "Texture3D.h"
#include "Mesh3D.h"

class RenderManager3D
{
private:
	RenderManager3D();
protected:
	
		RenderManager3D(RenderManager3D const&)  = delete;
        void operator=(RenderManager3D const&)  = delete;
public:
	static RenderManager3D& GetInstance()
        {
            static RenderManager3D instance;			
            return instance;
        }
	D3D10_DRIVER_TYPE driverType;
	ID3D10Device* d3dDevice;
	IDXGISwapChain* swapChain;
	ID3D10RenderTargetView* renderTargetView;
	D3D10_TEXTURE2D_DESC backBufferInfo;
	HINSTANCE hInst;
	HWND hwnd;
	D3D10_VIEWPORT viewPort;

	Sprites3D sprites;
	std::list<Texture3D*> textures;
	std::list<Mesh3D*> meshes;
	~RenderManager3D(){};
	bool init(HINSTANCE hInstance, unsigned int width, unsigned int height, bool fullScreen, const char* windowTitle);
	void free();
	bool update();
	
};

