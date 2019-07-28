#pragma once

#define SAFE_RELEASE(x) \
	if(x != NULL)		\
	{					\
		x->Release();	\
		x = NULL;		\
	}					\


#define MAX_SPRITES 4096

#include <d3d10.h>
#include <d3dx10.h>
#include <string>
#include <list>

struct Vertex 
{
	D3DXVECTOR3 Pos;
	D3DXVECTOR4 Color;
};

