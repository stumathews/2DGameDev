#include "RenderManager3D.h"

RenderManager3D::RenderManager3D()
{
	driverType = D3D10_DRIVER_TYPE_NULL;
	d3dDevice = NULL;
	swapChain = NULL;
	renderTargetView = NULL;
	hInst = NULL;
	hwnd = NULL;
	ZeroMemory(&backBufferInfo, sizeof(D3D10_TEXTURE2D_DESC));
}

LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lparam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	switch(message)
	{
		case WM_PAINT:
			hdc = BeginPaint( hWnd, &ps);
			EndPaint(hWnd, &ps);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc( hWnd, message, wParam, lparam);
	}
	return 0;
}

bool RenderManager3D::init(HINSTANCE hInstance, unsigned int width, unsigned int height, bool fullScreen, const char* windowTitle)
{
	
	WNDCLASSEX wcex;
	ZeroMemory(&wcex, sizeof(WNDCLASSEX));
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbWndExtra = 0;
	wcex.cbClsExtra = 0;
	wcex.hInstance = hInstance;
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = "class_engine";
	if(!RegisterClassEx(&wcex))
		return false;

	hInst = hInstance;
	RECT rc = { 0, 0, width, height };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

	
	hwnd = CreateWindow( "class_engine", "Window Title goes here dude", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 
		rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance, NULL);

	if(!hwnd)
		return false;
	
	ShowWindow(hwnd, SW_SHOWNORMAL);

	HRESULT hr = S_OK;
	ZeroMemory(&rc, sizeof(RECT));
	GetClientRect(hwnd, &rc);
	UINT bufferWidth = rc.right - rc.left;
	UINT bufferHeight = rc.bottom - rc.top;

	UINT createDeviceFlags = 0;
	D3D10_DRIVER_TYPE driverTypes[] = 
	{
		D3D10_DRIVER_TYPE_HARDWARE,
		D3D10_DRIVER_TYPE_REFERENCE
	};

	UINT numDriverTypes = sizeof(driverTypes) / sizeof(driverTypes[0]);
	DXGI_SWAP_CHAIN_DESC scd;
	ZeroMemory(&scd, sizeof(scd));
	scd.BufferCount = 1;
	scd.BufferDesc.Width = bufferWidth;
	scd.BufferDesc.Height = bufferHeight;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.RefreshRate.Numerator = 60;
	scd.BufferDesc.RefreshRate.Denominator = 1;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.OutputWindow = hwnd;
	scd.SampleDesc.Count = 1;
	scd.SampleDesc.Quality = 0;
	scd.Windowed = TRUE;

	UINT VERSION = D3D10_SDK_VERSION;
	for( UINT i = 0; i < numDriverTypes; i++)
	{
		driverType = driverTypes[i];
		hr = D3D10CreateDeviceAndSwapChain(NULL, driverType, NULL, createDeviceFlags, VERSION, &scd, &swapChain, &d3dDevice);
		if(SUCCEEDED(hr))
			break;
	}
	if(FAILED(hr))
		return false;
	
	ID3D10Texture2D* backBuffer;
	hr = swapChain->GetBuffer(0, __uuidof(ID3D10Texture2D), (LPVOID*) &backBuffer);
	if(FAILED(hr))
		false;
	
	backBuffer->GetDesc(&backBufferInfo);

	hr = d3dDevice->CreateRenderTargetView(backBuffer, NULL, &renderTargetView);
	backBuffer->Release();
	if(FAILED(hr))
		return false;

	d3dDevice->OMSetRenderTargets(1, &renderTargetView, NULL);

	D3D10_VIEWPORT viewPort;
	viewPort.Width = width;
	viewPort.Height = height;
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;
	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;

	
	d3dDevice->RSSetViewports( 1, &viewPort);
	this->viewPort = viewPort;

	return true;


}

void RenderManager3D::free()
{
	for( auto texture : textures) {
		texture->free();
	}
	for(auto mesh : meshes){ 
		mesh->free();
	}
	sprites.free();
	if(d3dDevice)
		d3dDevice->ClearState();

	SAFE_RELEASE(renderTargetView);
	SAFE_RELEASE(swapChain);
	SAFE_RELEASE(d3dDevice);
}

bool RenderManager3D::update()
{
	float clearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; //rgba

	d3dDevice->ClearRenderTargetView(renderTargetView, clearColor);
	sprites.update();
	for( auto mesh : meshes)
	{
		mesh->update();
	}
	swapChain->Present(0,0);
	return true;
}

