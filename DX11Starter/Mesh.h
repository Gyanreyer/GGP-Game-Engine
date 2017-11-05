#pragma once
#include <d3d11.h> //Included to gain access to direct X Variables
#include "Vertex.h"	//Included to gain access to vertex struct

//Add colliderType and colliderOffset
class Mesh
{
public:
	Mesh(
		Vertex* vertices,			//Holds an array of Vertex objects that are used for mesh
		int numVertices,			//Tells Mesh how many Vertex objects are in mesh's vertices array
		unsigned int* indicies,		//Holds array of indices for telling the order to draw vertices in
		int numIndices,				//Tells Mesh how many indices are in Index Array
		ID3D11Device* drawDevice);	//Reference to directX 11 Device need to create buffers
	Mesh(
		ID3D11Buffer* vertexBuffer,	//Buffer that holds the vertex data
		int vertCount,				//Tells mesh how many vertices there are in mesh
		ID3D11Buffer* indexBuffer,	//Buffer holds the indices data
		int indexNum				//Tells mesh how many indices there are
	);		
	~Mesh(); //Releases DirectX Buffers

	//Getters for private variables
	ID3D11Buffer* GetVertexBuffer();
	ID3D11Buffer* GetIndexBuffer();
	int GetVerticesCount();
	int GetIndexCount();

private:
	ID3D11Buffer* vertexBufferPtr;
	ID3D11Buffer* indexBufferPtr;
	int vertexCount;
	int indexCount;
};

