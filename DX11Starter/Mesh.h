#pragma once

#include "Vertex.h"
#include <d3d11.h>
#include <fstream>
#include <vector>

using namespace DirectX;

class Mesh
{
public:
	Mesh(char* objFile, ID3D11Device * device);
	Mesh(int numSides, ID3D11Device * device);
	~Mesh();	
	
	ID3D11Buffer * GetVertexBuffer();
	ID3D11Buffer * GetIndexBuffer();
	int GetIndexCount();

private:
	ID3D11Buffer * vertexBuffer;
	ID3D11Buffer * indexBuffer;
	
	int numIndices;
	int numVerts;

	void createBuffers(Vertex* vertices, UINT* indices, ID3D11Device * device);
};

