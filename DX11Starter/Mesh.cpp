#include "Mesh.h"
#include <fstream>
#include <vector>
#include <DirectXMath.h>

// For the DirectX Math library
using namespace DirectX;

//Defining and drawing our own shapes
Mesh::Mesh(Vertex * vertices, int numVertices, unsigned int * indicies, int numIndices, ID3D11Device * drawDevice)
{
	//Tangents for normal mapping
	CalculateTangents(vertices, numVertices, indicies, numIndices);

	//Create DX11 Vertex Buffer
	D3D11_BUFFER_DESC vbd;	//Vertex Buffer Description (Only needed when we create buffer)
	vbd.Usage = D3D11_USAGE_IMMUTABLE;	//Tells DirectX that we will not change data stored in buffer
	vbd.ByteWidth = sizeof(Vertex) * numVertices;	//tells DX how many Vertices will be stored in buffer so it can allocate the neccesary memory required
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;	//Tells DX that this is a Vertex Buffer
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA ivd; // creates Struct to put inital vertex data into buffer
	ivd.pSysMem = vertices; 

	drawDevice->CreateBuffer(&vbd, &ivd, &vertexBufferPtr);
	
	vertexCount = numVertices; //update the vertex count

	//Create DX11 Index Buffer
	D3D11_BUFFER_DESC ibd;	//Index Buffer Description (Only needed when we create buffer)
	ibd.Usage = D3D11_USAGE_IMMUTABLE;	//Tells DirectX that we will not change data stored in buffer
	ibd.ByteWidth = sizeof(int) * numIndices; //tells DX how many Indices will be stored in buffer so it can allocate the neccesary memory required
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER; //Tells DX that this is a Index Buffer
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA initialIndexData; //creates struct to put initial index data into Buffer
	initialIndexData.pSysMem = indicies;

	drawDevice->CreateBuffer(&ibd, &initialIndexData, &indexBufferPtr);

	indexCount = numIndices; //update index count
}

//Creating a model in AssetManager without a collider
Mesh::Mesh(ID3D11Buffer * vertexBuffer, int vertCount, ID3D11Buffer * indexBuffer, int indexNum)
{
	vertexBufferPtr = vertexBuffer;
	vertexCount = vertCount;
	indexBufferPtr = indexBuffer;
	indexCount = indexNum;
	collType = NONE;
	isColliderOffset = false;
}

//Creating a model in AssetManager with a collider
Mesh::Mesh(ID3D11Buffer * vertexBuffer, int vertCount, ID3D11Buffer * indexBuffer, int indexNum, ColliderType cType, bool isCollOffset) 
	: Mesh(vertexBuffer, vertCount, indexBuffer, indexNum)
{
	collType = cType;
	isColliderOffset = isCollOffset;
}

Mesh::~Mesh()
{
	//Call Release on Buffers To clean up memory when Mesh is done
	vertexBufferPtr->Release();
	indexBufferPtr->Release();
}

ID3D11Buffer * Mesh::GetVertexBuffer()
{
	return vertexBufferPtr;
}

ID3D11Buffer * Mesh::GetIndexBuffer()
{
	return indexBufferPtr;
}

ColliderType Mesh::GetColliderType()
{
	return collType;
}

bool Mesh::GetIsColliderOffset()
{
	return isColliderOffset;
}

int Mesh::GetVerticesCount()
{
	return vertexCount;
}

int Mesh::GetIndexCount()
{
	return indexCount;
}

// Calculates the tangents of the vertices in a mesh
// Code adapted from: http://www.terathon.com/code/tangent.html
void Mesh::CalculateTangents(Vertex* verts, int numVerts, unsigned int* indices, int numIndices)
{
	// Reset tangents
	for (int i = 0; i < numVerts; i++)
	{
		verts[i].Tangent = XMFLOAT3(0, 0, 0);
	}

	// Calculate tangents one whole triangle at a time
	for (int i = 0; i < numVerts;)
	{
		// Grab indices and vertices of first triangle
		unsigned int i1 = indices[i++];
		unsigned int i2 = indices[i++];
		unsigned int i3 = indices[i++];
		Vertex* v1 = &verts[i1];
		Vertex* v2 = &verts[i2];
		Vertex* v3 = &verts[i3];

		// Calculate vectors relative to triangle positions
		float x1 = v2->Position.x - v1->Position.x;
		float y1 = v2->Position.y - v1->Position.y;
		float z1 = v2->Position.z - v1->Position.z;

		float x2 = v3->Position.x - v1->Position.x;
		float y2 = v3->Position.y - v1->Position.y;
		float z2 = v3->Position.z - v1->Position.z;

		// Do the same for vectors relative to triangle uv's
		float s1 = v2->UV.x - v1->UV.x;
		float t1 = v2->UV.y - v1->UV.y;

		float s2 = v3->UV.x - v1->UV.x;
		float t2 = v3->UV.y - v1->UV.y;

		// Create vectors for tangent calculation
		float r = 1.0f / (s1 * t2 - s2 * t1);

		float tx = (t2 * x1 - t1 * x2) * r;
		float ty = (t2 * y1 - t1 * y2) * r;
		float tz = (t2 * z1 - t1 * z2) * r;

		// Adjust tangents of each vert of the triangle
		v1->Tangent.x += tx;
		v1->Tangent.y += ty;
		v1->Tangent.z += tz;

		v2->Tangent.x += tx;
		v2->Tangent.y += ty;
		v2->Tangent.z += tz;

		v3->Tangent.x += tx;
		v3->Tangent.y += ty;
		v3->Tangent.z += tz;
	}

	// Ensure all of the tangents are orthogonal to the normals
	for (int i = 0; i < numVerts; i++)
	{
		// Grab the two vectors
		XMVECTOR normal = XMLoadFloat3(&verts[i].Normal);
		XMVECTOR tangent = XMLoadFloat3(&verts[i].Tangent);

		// Use Gram-Schmidt orthogonalize
		tangent = XMVector3Normalize(
			tangent - normal * XMVector3Dot(normal, tangent));

		// Store the tangent
		XMStoreFloat3(&verts[i].Tangent, tangent);
	}
}