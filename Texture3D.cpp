#include "Texture3D.h"
#include "RenderManager3D.h"
Texture3D::Texture3D()
{
}

bool Texture3D::loadFromFile(std::wstring fileName)
{
	size_t   i;
	char filepath[MAX_PATH];
	wcstombs_s(&i, filepath, fileName.c_str(), MAX_PATH);
	auto res = D3DX10GetImageInfoFromFile( filepath, NULL, fileInfo, NULL);
	if(SUCCEEDED(res))
	{
		RenderManager3D& renderManager = RenderManager3D::GetInstance();
		ID3D10Resource* tmpTextureResource;

		HRESULT hr = D3DX10CreateTextureFromFile(renderManager.d3dDevice, filepath, NULL, NULL, &tmpTextureResource, NULL);
		if(FAILED(hr))	
			return false;
		
		tmpTextureResource->QueryInterface(__uuidof(ID3D10Texture2D), (LPVOID*)textureResource);
		tmpTextureResource->Release();

		textureInterface->GetDesc(&textureDescription);
		isLoaded = true;
		if(!isOwned){
			renderManager.textures.push_back(this);
			isOwned = true;
		}
		return true;
	}
	return false;
}

bool Texture3D::free()
{
	return false;
}

void Texture3D::blitRectToBackBuffer(RECT rect, int x, int y)
{
	RenderManager3D& rendermanager = RenderManager3D::GetInstance();
	ID3D10Texture2D* buffer;
	rendermanager.swapChain->GetBuffer(0, __uuidof(ID3D10Texture2D), (LPVOID*) &buffer);

	if(((rect.right-rect.left) <= rendermanager.backBufferInfo.Width) && (rect.bottom-rect.top) <= rendermanager.backBufferInfo.Height)
	{
		D3D10_BOX crop;
		crop.left = rect.left;
		crop.right = rect.right;
		crop.top = rect.top;
		crop.bottom =  rect.bottom;
		crop.front = 0;
		crop.back = 1;

		rendermanager.d3dDevice->CopySubresourceRegion(buffer, x, y, 0, 0,  textureInterface, 0, &crop);

	}
	buffer->Release();
}
