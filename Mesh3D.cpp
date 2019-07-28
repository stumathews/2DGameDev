#include "Mesh3D.h"
#include "RenderManager3D.h"
Mesh3D::Mesh3D()
{
	effect = NULL;
	technique = NULL;
	mesh = NULL;
	vertexlayout = NULL;
	worldVariable = NULL;
	viewVariable = NULL;
	projectionVariable = NULL;

	D3DXMatrixIdentity(&world);
	D3DXMatrixIdentity(&view);
	D3DXMatrixIdentity(&projection);

}

void Mesh3D::create()
{

	D3DX10CreateEffectFromFile("MyShader.fx", NULL, NULL, "fx_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, RenderManager3D::GetInstance().d3dDevice, NULL, NULL, &effect, NULL, NULL);
	
	technique = effect->GetTechniqueByName("Render");

	D3D10_INPUT_ELEMENT_DESC layout[] = 
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0}
	};

	UINT numElements = sizeof(layout) / sizeof(layout[0]);

	D3D10_PASS_DESC passDescription;
	technique->GetPassByIndex(0)->GetDesc(&passDescription);
		
	RenderManager3D::GetInstance().d3dDevice->CreateInputLayout(layout, numElements, passDescription.pIAInputSignature, passDescription.IAInputSignatureSize, &vertexlayout);
	RenderManager3D::GetInstance().d3dDevice->IASetInputLayout(vertexlayout);
		
	 Vertex v[] =
		{
			{ D3DXVECTOR3( -1.0f, 1.0f, -1.0f ), D3DXVECTOR4( 0.0f, 0.0f, 1.0f, 1.0f ) },
			{ D3DXVECTOR3( 1.0f, 1.0f, -1.0f ), D3DXVECTOR4( 0.0f, 1.0f, 0.0f, 1.0f ) },
			{ D3DXVECTOR3( 1.0f, 1.0f, 1.0f ), D3DXVECTOR4( 0.0f, 1.0f, 1.0f, 1.0f ) },
			{ D3DXVECTOR3( -1.0f, 1.0f, 1.0f ), D3DXVECTOR4( 1.0f, 0.0f, 0.0f, 1.0f ) },
			{ D3DXVECTOR3( -1.0f, -1.0f, -1.0f ), D3DXVECTOR4( 1.0f, 0.0f, 1.0f, 1.0f ) },
			{ D3DXVECTOR3( 1.0f, -1.0f, -1.0f ), D3DXVECTOR4( 1.0f, 1.0f, 0.0f, 1.0f ) },
			{ D3DXVECTOR3( 1.0f, -1.0f, 1.0f ), D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f ) },
			{ D3DXVECTOR3( -1.0f, -1.0f, 1.0f ), D3DXVECTOR4( 0.0f, 0.0f, 0.0f, 1.0f ) },
		};

	UINT numVertices = sizeof(v)/sizeof(v[0]);
	DWORD i[] = 
	{
		3,1,0,
		2,1,3,

		0,5,4,
		1,5,0,

		3,4,7,
		0,4,3,

		1,6,5,
		2,6,1,

		2,7,6,
		3,7,2,

		6,4,5,
		7,4,6,

	};

	UINT numIndices = sizeof(i)/sizeof(i[0]);
	auto meshCreated = D3DX10CreateMesh(RenderManager3D::GetInstance().d3dDevice, layout, numElements, "POSITION", numVertices, numIndices/3, D3DX10_MESH_32_BIT, &mesh);
	if(SUCCEEDED(meshCreated))
	{
		mesh->SetVertexData(0,v);
		mesh->SetIndexData(i, numIndices);
		mesh->CommitToDevice();
	}
}

void Mesh3D::update()
{
	worldVariable = effect->GetVariableByName("World")->AsMatrix();
	viewVariable = effect->GetVariableByName("View")->AsMatrix();
	projectionVariable = effect->GetVariableByName("Projection")->AsMatrix();

	D3DXVECTOR3 eye(0.0f, 1.0f, -5.0f);
	D3DXVECTOR3 at(0.0f, 1.0f, 0.0f);
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);

	D3DXMatrixLookAtLH(&view, &eye, &at, &up);

	D3DXMatrixPerspectiveFovLH(&projection, (float) D3DX_PI * 0.5f, RenderManager3D::GetInstance().viewPort.Width / (float) RenderManager3D::GetInstance().viewPort.Height, 0.1f, 100.0f);

	worldVariable->SetMatrix(world);
	viewVariable->SetMatrix(view);
	projectionVariable->SetMatrix(projection);

	D3D10_TECHNIQUE_DESC techniqueDescrition;
	technique->GetDesc(&techniqueDescrition);
	for(UINT p = 0; p < techniqueDescrition.Passes; ++p)
	{
		technique->GetPassByIndex(p)->Apply(0);
		mesh->DrawSubset(0);
	}


}

void Mesh3D::free()
{
}
