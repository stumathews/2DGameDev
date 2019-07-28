#pragma once
#include "COMUtils.h"
#include "Texture3D.h"
class Sprites3D
{
private:
protected:
	D3DX10_SPRITE spriteInfo[MAX_SPRITES];
	D3DXVECTOR2 Positions[MAX_SPRITES];
	D3DXVECTOR2 Scales[MAX_SPRITES];
	Texture3D textures[MAX_SPRITES];
	ID3DX10Sprite* spriteInterface;
public:
	unsigned int numSprites;
	Sprites3D();
	void setUVCoords(int id, float left, float top, float width, float height);
	void setColor(int id, float r, float g, float b, float a=1.0f);
	void setPos(int id, float x, float y);
	void setScale(int id, float x, float y);
	bool loadFromFile(int id, std::wstring filename);
	void free();
	void update();
};

