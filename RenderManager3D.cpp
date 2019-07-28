#include "RenderManager3D.h"

RenderManager3D::RenderManager3D()
{
	driverType = D3D10_DRIVER_TYPE_NULL;
	d3dDevice = NULL;	
	renderTargetView = NULL;
	hInst = NULL;
	hwnd = NULL;
	
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

class Window3D
{
public:

	
	// Define a Window to draw in
	Window3D(HINSTANCE hInstance, int width, int height) : Window3D()
	{
		rect = { 0, 0, width, height };
		this->hInstance = hInstance;
		// Define a Window to draw in		
		
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = WndProc;
		wcex.cbWndExtra = 0;
		wcex.cbClsExtra = 0;
		wcex.hInstance = hInstance;
		wcex.lpszMenuName = NULL;
		wcex.lpszClassName = "class_engine";
	}
	WORD RegisterClass()
	{
		if(!RegisterClassEx(&wcex))
			return false;
	}

	bool Initialize()
	{
		// Register the window with Windows
		if(!RegisterClass())
			return false;
	
		// Calculate and get the size of a WS_OVERLAPPEDWINDOW window
		
		AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
		initialized = true;
		return true;
	}

	// Create the window as a Hwnd
	HWND Create()
	{
		if(!initialized)
			return NULL;
		// Calculate and get the size of a WS_OVERLAPPEDWINDOW window
		AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
	
		// Create the window as a Hwnd
		hwnd = CreateWindow( "class_engine", "Window Title goes here dude", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top, NULL, NULL, hInstance, NULL);
		return hwnd;
	}
	
	// Get drawable region of window (ClientRect)
	RECT GetWindowClientArea()
	{
		ZeroMemory(&rect, sizeof(RECT));
		GetClientRect(hwnd, &rect);
		return rect;
	}
	void Show()
	{
		ShowWindow(hwnd, SW_SHOWNORMAL);
	}
private:
	WNDCLASSEX wcex;
	HWND hwnd;
	RECT rect = {};
	HINSTANCE hInstance;
	bool initialized = false;
	Window3D()		
	{		
		ZeroMemory(&wcex, sizeof(WNDCLASSEX));
	}

};


bool RenderManager3D::Initialize(HINSTANCE hInstance, unsigned int width, unsigned int height, bool fullScreen, const char* windowTitle)
{
	HRESULT hr = S_OK;
	hInst = hInstance;
	UINT VERSION = D3D10_SDK_VERSION;
	
	// Setup a Window for this App
	Window3D window(hInstance, width, height);
	if(!window.Initialize())
		return false;	
	
	// Get window hwnd
	if(!(hwnd = window.Create()))
		return false;	
	
	window.Show();	

	RECT clientArea = window.GetWindowClientArea();

	// Create a SwapChain
	swapChain = MySwapChain(clientArea.right - clientArea.left, clientArea.bottom - clientArea.top, hwnd);
			
	// See which driver type we can get to create the device and swap chain with
	D3D10_DRIVER_TYPE driverTypes[] = 
	{
		D3D10_DRIVER_TYPE_HARDWARE,
		D3D10_DRIVER_TYPE_REFERENCE
	};
	UINT numDriverTypes = sizeof(driverTypes) / sizeof(driverTypes[0]);
	UINT createDeviceFlags = 0;
	for( UINT i = 0; i < numDriverTypes; i++)
	{
		// Swap chain is created/populated here
		hr = D3D10CreateDeviceAndSwapChain(NULL, driverTypes[i], NULL, createDeviceFlags, VERSION, &swapChain.GetDescription(), &swapChain.D3DInterface, &d3dDevice);
		if(SUCCEEDED(hr))
			break;
	}

	if(FAILED(hr))
		return false;

	// Create a render target view from the back buffer
	hr = d3dDevice->CreateRenderTargetView(swapChain.GetBackBufferAsTexture(), NULL, &renderTargetView);
	
	swapChain.GetBackBufferAsTexture()->Release();
	if(FAILED(hr))
		return false;

	// Register the render target view with the device
	d3dDevice->OMSetRenderTargets(1, &renderTargetView, NULL);

	// Define the viewport
	D3D10_VIEWPORT viewPort;
	viewPort.Width = width;
	viewPort.Height = height;
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;
	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;

	// Register the viewport with the device
	d3dDevice->RSSetViewports( 1, &viewPort);
	
	this->viewPort = viewPort;
	return true;
}

void RenderManager3D::free()
{
	// Free textures
	for( auto texture : textures) {
		texture->free();
	}
	// Free meshes
	for(auto mesh : meshes){ 
		mesh->free();
	}

	// Free sprites
	sprites.free();

	// Clear Device state
	if(d3dDevice)
		d3dDevice->ClearState();
	
	// Free render target, swapchain and device
	SAFE_RELEASE(renderTargetView);
	SAFE_RELEASE(swapChain.D3DInterface);
	SAFE_RELEASE(d3dDevice);
}

bool RenderManager3D::update()
{
	// Blank out the drawing area
	d3dDevice->ClearRenderTargetView(renderTargetView, clearColor);
	
	// Update all drawings
	sprites.update();
	for( auto mesh : meshes)
	{
		mesh->update();
	}
	
	// Show the updated drawings
	swapChain.D3DInterface->Present(0,0);
	return true;
}

