#include "AssetManager.h"
#include <fstream>
#include <vector>
#include <string>
#include <DirectXMath.h>
#include "WICTextureLoader.h" //used for texture loading
#include "DDSTextureLoader.h" //used for cube map texture loading

//For the DirectX Math Library
using namespace DirectX;

AssetManager::AssetManager()
{
	materialLibrary = std::unordered_map<char*, Material*>();
}


AssetManager & AssetManager::getInstance()
{
	static AssetManager instance;
	return instance;
}

AssetManager::~AssetManager()
{
	//Loop through vertex Shader Library and release Vertex Shader DX Resources
	for (auto i = vertexShaderLibrary.begin(); i != vertexShaderLibrary.end(); i++) {
		delete i->second;
	}

	//Loop through pixel Shader Library and release Pixel Shader DX Resources
	for (auto i = pixelShaderLibrary.begin(); i != pixelShaderLibrary.end(); i++) {
		delete i->second;
	}

	//Loop through Texture Library and release Texture DX Resources
	for (auto i = textureLibrary.begin(); i != textureLibrary.end(); i++) {
		i->second->Release();
	}
	//Loop through Texture Sampler Library and release Sampler DX Resources
	for (auto i = textureSamplerLibrary.begin(); i != textureSamplerLibrary.end(); i++) {
		i->second->Release();
	}
	//Loops through material library and deletes all material Pointers
	for (auto i = materialLibrary.begin(); i != materialLibrary.end(); i++) {
		Material* mat;
		mat = i->second;
		delete mat;
	}

	//Loop through Mesh Library and delete all mesh Pointers
	for (auto i = meshLibrary.begin(); i != meshLibrary.end(); i++) {
		Mesh* mesh;
		mesh = i->second;
		delete mesh;
	}
}

//Meshes without colliders
void AssetManager::ImportMesh(char* meshName, char * meshFile, ID3D11Device* drawDevice)
{
	/*Model Loading Code Provide by Chris Casciolli*/

	// File input object
	std::ifstream obj(meshFile);

	// Check for successful open
	if (!obj.is_open())
		return;

	// Variables used while reading the file
	std::vector<XMFLOAT3> positions;     // Positions from the file
	std::vector<XMFLOAT3> normals;       // Normals from the file
	std::vector<XMFLOAT2> uvs;           // UVs from the file
	std::vector<Vertex> verts;           // Verts we're assembling
	std::vector<UINT> indices;           // Indices of these verts
	unsigned int vertCounter = 0;        // Count of vertices/indices
	char chars[100];                     // String for line reading

	// Still have data left?
	while (obj.good())
	{
		// Get the line (100 characters should be more than enough)
		obj.getline(chars, 100);

		// Check the type of line
		if (chars[0] == 'v' && chars[1] == 'n')
		{
			// Read the 3 numbers directly into an XMFLOAT3
			XMFLOAT3 norm;
			sscanf_s(
				chars,
				"vn %f %f %f",
				&norm.x, &norm.y, &norm.z);

			// Add to the list of normals
			normals.push_back(norm);
		}
		else if (chars[0] == 'v' && chars[1] == 't')
		{
			// Read the 2 numbers directly into an XMFLOAT2
			XMFLOAT2 uv;
			sscanf_s(
				chars,
				"vt %f %f",
				&uv.x, &uv.y);

			// Add to the list of uv's
			uvs.push_back(uv);
		}
		else if (chars[0] == 'v')
		{
			// Read the 3 numbers directly into an XMFLOAT3
			XMFLOAT3 pos;
			sscanf_s(
				chars,
				"v %f %f %f",
				&pos.x, &pos.y, &pos.z);

			// Add to the positions
			positions.push_back(pos);
		}
		else if (chars[0] == 'f')
		{
			// Read the face indices into an array
			unsigned int i[12];
			int facesRead = sscanf_s(
				chars,
				"f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d",
				&i[0], &i[1], &i[2],
				&i[3], &i[4], &i[5],
				&i[6], &i[7], &i[8],
				&i[9], &i[10], &i[11]);

			// - Create the verts by looking up
			//    corresponding data from vectors
			// - OBJ File indices are 1-based, so
			//    they need to be adusted
			Vertex v1 = Vertex();
			v1.Position = positions[i[0] - 1];
			v1.UV = uvs[i[1] - 1];
			v1.Normal = normals[i[2] - 1];

			Vertex v2 = Vertex();
			v2.Position = positions[i[3] - 1];
			v2.UV = uvs[i[4] - 1];
			v2.Normal = normals[i[5] - 1];

			Vertex v3 = Vertex();
			v3.Position = positions[i[6] - 1];
			v3.UV = uvs[i[7] - 1];
			v3.Normal = normals[i[8] - 1];

			// The model is most likely in a right-handed space,
			// especially if it came from Maya.  We want to convert
			// to a left-handed space for DirectX.  This means we 
			// need to:
			//  - Invert the Z position
			//  - Invert the normal's Z
			//  - Flip the winding order
			// We also need to flip the UV coordinate since DirectX
			// defines (0,0) as the top left of the texture, and many
			// 3D modeling packages use the bottom left as (0,0)

			// Flip the UV's since they're probably "upside down"
			v1.UV.y = 1.0f - v1.UV.y;
			v2.UV.y = 1.0f - v2.UV.y;
			v3.UV.y = 1.0f - v3.UV.y;

			// Flip Z (LH vs. RH)
			v1.Position.z *= -1.0f;
			v2.Position.z *= -1.0f;
			v3.Position.z *= -1.0f;

			// Flip normal Z
			v1.Normal.z *= -1.0f;
			v2.Normal.z *= -1.0f;
			v3.Normal.z *= -1.0f;

			// Add the verts to the vector (flipping the winding order)
			verts.push_back(v1);
			verts.push_back(v3);
			verts.push_back(v2);

			// Add three more indices
			indices.push_back(vertCounter); vertCounter += 1;
			indices.push_back(vertCounter); vertCounter += 1;
			indices.push_back(vertCounter); vertCounter += 1;

			// Was there a 4th face?
			if (facesRead == 12)
			{
				// Make the last vertex
				Vertex v4 = Vertex();
				v4.Position = positions[i[9] - 1];
				v4.UV = uvs[i[10] - 1];
				v4.Normal = normals[i[11] - 1];

				// Flip the UV, Z pos and normal
				v4.UV.y = 1.0f - v4.UV.y;
				v4.Position.z *= -1.0f;
				v4.Normal.z *= -1.0f;

				// Add a whole triangle (flipping the winding order)
				verts.push_back(v1);
				verts.push_back(v4);
				verts.push_back(v3);

				// Add three more indices
				indices.push_back(vertCounter); vertCounter += 1;
				indices.push_back(vertCounter); vertCounter += 1;
				indices.push_back(vertCounter); vertCounter += 1;
			}
		}
	}

	// Close the file and create the actual buffers
	obj.close();


	// - At this point, "verts" is a vector of Vertex structs, and can be used
	//    directly to create a vertex buffer:  &verts[0] is the address of the first vert
	//
	// - The vector "indices" is similar. It's a vector of unsigned ints and
	//    can be used directly for the index buffer: &indices[0] is the address of the first int
	//
	// - "vertCounter" is BOTH the number of vertices and the number of indices
	// - Yes, the indices are a bit redundant here (one per vertex).  Could you skip using
	//    an index buffer in this case?  Sure!  Though, if your mesh class assumes you have
	//    one, you'll need to write some extra code to handle cases when you don't.
	//Create DX11 Vertex Buffer
	//----------------------End-Chris's-Code-----------------------------------------------------

	Mesh* mesh = new Mesh(&verts[0], vertCounter, &indices[0], vertCounter, drawDevice); //Create a mesh that doesn't have a collider
	StoreMesh(meshName, mesh); //Stores mesh into mesh library using associated key
}

//Meshes with colliders
void AssetManager::ImportMesh(char* meshName, char * meshFile, ID3D11Device* drawDevice, ColliderType cType, bool isCollOffset)
{
	/*Model Loading Code Provide by Chris Casciolli*/

	// File input object
	std::ifstream obj(meshFile);

	// Check for successful open
	if (!obj.is_open())
		return;

	// Variables used while reading the file
	std::vector<XMFLOAT3> positions;     // Positions from the file
	std::vector<XMFLOAT3> normals;       // Normals from the file
	std::vector<XMFLOAT2> uvs;           // UVs from the file
	std::vector<Vertex> verts;           // Verts we're assembling
	std::vector<UINT> indices;           // Indices of these verts
	unsigned int vertCounter = 0;        // Count of vertices/indices
	char chars[100];                     // String for line reading

	// Still have data left?
	while (obj.good())
	{
		// Get the line (100 characters should be more than enough)
		obj.getline(chars, 100);

		// Check the type of line
		if (chars[0] == 'v' && chars[1] == 'n')
		{
			// Read the 3 numbers directly into an XMFLOAT3
			XMFLOAT3 norm;
			sscanf_s(
				chars,
				"vn %f %f %f",
				&norm.x, &norm.y, &norm.z);

			// Add to the list of normals
			normals.push_back(norm);
		}
		else if (chars[0] == 'v' && chars[1] == 't')
		{
			// Read the 2 numbers directly into an XMFLOAT2
			XMFLOAT2 uv;
			sscanf_s(
				chars,
				"vt %f %f",
				&uv.x, &uv.y);

			// Add to the list of uv's
			uvs.push_back(uv);
		}
		else if (chars[0] == 'v')
		{
			// Read the 3 numbers directly into an XMFLOAT3
			XMFLOAT3 pos;
			sscanf_s(
				chars,
				"v %f %f %f",
				&pos.x, &pos.y, &pos.z);

			// Add to the positions
			positions.push_back(pos);
		}
		else if (chars[0] == 'f')
		{
			// Read the face indices into an array
			unsigned int i[12];
			int facesRead = sscanf_s(
				chars,
				"f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d",
				&i[0], &i[1], &i[2],
				&i[3], &i[4], &i[5],
				&i[6], &i[7], &i[8],
				&i[9], &i[10], &i[11]);

			// - Create the verts by looking up
			//    corresponding data from vectors
			// - OBJ File indices are 1-based, so
			//    they need to be adusted
			Vertex v1 = Vertex();
			v1.Position = positions[i[0] - 1];
			v1.UV = uvs[i[1] - 1];
			v1.Normal = normals[i[2] - 1];

			Vertex v2 = Vertex();
			v2.Position = positions[i[3] - 1];
			v2.UV = uvs[i[4] - 1];
			v2.Normal = normals[i[5] - 1];

			Vertex v3 = Vertex();
			v3.Position = positions[i[6] - 1];
			v3.UV = uvs[i[7] - 1];
			v3.Normal = normals[i[8] - 1];

			// The model is most likely in a right-handed space,
			// especially if it came from Maya.  We want to convert
			// to a left-handed space for DirectX.  This means we 
			// need to:
			//  - Invert the Z position
			//  - Invert the normal's Z
			//  - Flip the winding order
			// We also need to flip the UV coordinate since DirectX
			// defines (0,0) as the top left of the texture, and many
			// 3D modeling packages use the bottom left as (0,0)

			// Flip the UV's since they're probably "upside down"
			v1.UV.y = 1.0f - v1.UV.y;
			v2.UV.y = 1.0f - v2.UV.y;
			v3.UV.y = 1.0f - v3.UV.y;

			// Flip Z (LH vs. RH)
			v1.Position.z *= -1.0f;
			v2.Position.z *= -1.0f;
			v3.Position.z *= -1.0f;

			// Flip normal Z
			v1.Normal.z *= -1.0f;
			v2.Normal.z *= -1.0f;
			v3.Normal.z *= -1.0f;

			// Add the verts to the vector (flipping the winding order)
			verts.push_back(v1);
			verts.push_back(v3);
			verts.push_back(v2);

			// Add three more indices
			indices.push_back(vertCounter); vertCounter += 1;
			indices.push_back(vertCounter); vertCounter += 1;
			indices.push_back(vertCounter); vertCounter += 1;

			// Was there a 4th face?
			if (facesRead == 12)
			{
				// Make the last vertex
				Vertex v4 = Vertex();
				v4.Position = positions[i[9] - 1];
				v4.UV = uvs[i[10] - 1];
				v4.Normal = normals[i[11] - 1];

				// Flip the UV, Z pos and normal
				v4.UV.y = 1.0f - v4.UV.y;
				v4.Position.z *= -1.0f;
				v4.Normal.z *= -1.0f;

				// Add a whole triangle (flipping the winding order)
				verts.push_back(v1);
				verts.push_back(v4);
				verts.push_back(v3);

				// Add three more indices
				indices.push_back(vertCounter); vertCounter += 1;
				indices.push_back(vertCounter); vertCounter += 1;
				indices.push_back(vertCounter); vertCounter += 1;
			}
		}
	}

	// Close the file and create the actual buffers
	obj.close();


	// - At this point, "verts" is a vector of Vertex structs, and can be used
	//    directly to create a vertex buffer:  &verts[0] is the address of the first vert
	//
	// - The vector "indices" is similar. It's a vector of unsigned ints and
	//    can be used directly for the index buffer: &indices[0] is the address of the first int
	//
	// - "vertCounter" is BOTH the number of vertices and the number of indices
	// - Yes, the indices are a bit redundant here (one per vertex).  Could you skip using
	//    an index buffer in this case?  Sure!  Though, if your mesh class assumes you have
	//    one, you'll need to write some extra code to handle cases when you don't.
	//Create DX11 Vertex Buffer
	//----------------------End-Chris's-Code-----------------------------------------------------

	Mesh* mesh = new Mesh(&verts[0], vertCounter, &indices[0], vertCounter, drawDevice, cType, isCollOffset);  //Create a mesh that does have a collider
	StoreMesh(meshName, mesh); //Stores mesh into mesh library using associated key
}

void AssetManager::ImportTexture(char * textureName, const wchar_t* textureFile, ID3D11Device* device, ID3D11DeviceContext* context)
{
	ID3D11ShaderResourceView* texture;
	HRESULT tResult = CreateWICTextureFromFile(device, context, textureFile, 0, &texture);
	if (tResult != S_OK) {
		printf("Texture is could not be loaded");
		return;
	}
	StoreTexture(textureName, texture);
}

void AssetManager::ImportCubeMapTexture(char * textureName, const wchar_t * textureFile, ID3D11Device * device)
{
	ID3D11ShaderResourceView* texture;
	HRESULT tResult = CreateDDSTextureFromFile(device, textureFile, 0, &texture);
	if (tResult != S_OK) {
		printf("Texture is could not be loaded");
		return;
	}
	StoreTexture(textureName, texture);
}

void AssetManager::CreateMaterial(char * materialName, SimpleVertexShader * vShader, SimplePixelShader * pShader, ID3D11ShaderResourceView * texture, ID3D11SamplerState * textureSampler)
{
	Material* material = new Material(vShader, pShader, texture, textureSampler);
	StoreMaterial(materialName, material);
}

void AssetManager::CreateMaterial(char * materialName, char * vShaderKey, char * pShaderKey, char * textureKey, char * samplerKey)
{
	SimpleVertexShader* vShader = vertexShaderLibrary[vShaderKey];
	SimplePixelShader* pShader = pixelShaderLibrary[pShaderKey];
	ID3D11ShaderResourceView* texture = textureLibrary[textureKey];
	ID3D11SamplerState* textureSampler = textureSamplerLibrary[samplerKey];

	Material* material = new Material(vShader, pShader, texture, textureSampler);
	StoreMaterial(materialName, material);
}

//Create material with normal
void AssetManager::CreateMaterial(char * materialName, char * vShaderKey, char * pShaderKey, char * textureKey, char * normalKey, char * samplerKey)
{
	SimpleVertexShader* vShader = vertexShaderLibrary[vShaderKey];
	SimplePixelShader* pShader = pixelShaderLibrary[pShaderKey];
	ID3D11ShaderResourceView* texture = textureLibrary[textureKey];
	ID3D11ShaderResourceView* normal = textureLibrary[normalKey];
	ID3D11SamplerState* textureSampler = textureSamplerLibrary[samplerKey];

	Material* material = new Material(vShader, pShader, texture, normal, textureSampler);
	StoreMaterial(materialName, material);
}

void AssetManager::StoreVShader(char * vShaderName, SimpleVertexShader * vShader)
{
	if (vertexShaderLibrary.count(vShaderName) == 0)
		vertexShaderLibrary[vShaderName] = vShader;
	else
	{
		std::string error = "ERROR: Vertex Shader Key: '";
		error += vShaderName;
		error += "' already exists in vShaderLibrary \n";
		printf(error.c_str());
	}
}

void AssetManager::StorePShader(char * pShaderName, SimplePixelShader * pShader)
{
	if(pixelShaderLibrary.count(pShaderName) == 0)
		pixelShaderLibrary[pShaderName] = pShader;
	else
	{
		std::string error = "ERROR: Pixel Shader Key: '";
		error += pShaderName;
		error += "' already exists in pShaderLibrary \n";
		printf(error.c_str());
	}
}

void AssetManager::StoreTexture(char * textureName, ID3D11ShaderResourceView * texture)
{
	if(textureLibrary.count(textureName) == 0)
		textureLibrary[textureName] = texture;
	else
	{
		std::string error = "ERROR: Texture Key: '";
		error += textureName;
		error += "' already exists in textureLibrary \n";
		printf(error.c_str());
	}
}

void AssetManager::StoreSampler(char * textSamplerName, ID3D11SamplerState * sampler)
{
	if(textureSamplerLibrary.count(textSamplerName) == 0)
		textureSamplerLibrary[textSamplerName] = sampler;
	else
	{
		std::string error = "ERROR: Texture Sampler Key: '";
		error += textSamplerName;
		error += "' already exists in samplerLibrary \n";
		printf(error.c_str());
	}
}

void AssetManager::StoreMaterial(char* matName, Material* materialPtr)
{
	if(materialLibrary.count(matName) == 0)
		materialLibrary[matName] = materialPtr;
	else 
	{
		std::string error = "ERROR: Material Key: '";
		error += matName;
		error += "' already exists in materialLibrary \n";
		printf(error.c_str());
	}
}

void AssetManager::StoreMesh(char * meshName, Mesh * meshPtr)
{
	if(meshLibrary.count(meshName) == 0)
		meshLibrary[meshName] = meshPtr;
	else
	{
		std::string error = "ERROR: Mesh Key: '";
		error += meshName;
		error += "' already exists in MeshLibrary \n";
		printf(error.c_str());
	}
}

SimpleVertexShader * AssetManager::GetVShader(char * vShaderName)
{
	if(vertexShaderLibrary.count(vShaderName) == 1) //Checks to make sure that VertexShaderKey is in library
		return vertexShaderLibrary[vShaderName];
	else
	{
		std::string noAssetError = "Error: '";
		noAssetError += vShaderName;
		noAssetError += "' Vertex Shader not found ";
		printf(noAssetError.c_str());
		return nullptr;
	}
}

SimplePixelShader * AssetManager::GetPShader(char * pShaderName)
{
	if(pixelShaderLibrary.count(pShaderName) == 1) //Checks to make sure that PixelShaderKey is in library
		return pixelShaderLibrary[pShaderName];
	else
	{
		std::string noAssetError = "Error: '";
		noAssetError += pShaderName;
		noAssetError += "' Pixel Shader not found ";
		printf(noAssetError.c_str());
		return nullptr;
	}
}

ID3D11ShaderResourceView * AssetManager::GetTexture(char * textureName)
{
	if(textureLibrary.count(textureName) == 1) //Checks to make sure that TextureKey is in library
		return textureLibrary[textureName];
	else
	{
		std::string noAssetError = "Error: '";
		noAssetError += textureName;
		noAssetError += "' Texture not found ";
		printf(noAssetError.c_str());
		return nullptr;
	}
}

ID3D11SamplerState * AssetManager::GetSampler(char * samplerName)
{
	if(textureSamplerLibrary.count(samplerName) == 1)  //Checks to make sure that SamplerhKey is in library
		return textureSamplerLibrary[samplerName];
	else
	{
		std::string noAssetError = "Error: '";
		noAssetError += samplerName;
		noAssetError += "' Texture Sampler not found ";
		printf(noAssetError.c_str());
		return nullptr;
	}
}

Material * AssetManager::GetMaterial(char * materialName)
{
	if(materialLibrary.count(materialName)==1)  //Checks to make sure that meshKey is in library
		return materialLibrary[materialName];
	else
	{
		std::string noAssetError = "Error: '";
		noAssetError += materialName;
		noAssetError += "' Material not found ";
		printf(noAssetError.c_str());
		return nullptr;
	}
}

Mesh * AssetManager::GetMesh(char * meshName)
{
	if(meshLibrary.count(meshName)==1)	//Checks to make sure that meshKey is in library
		return meshLibrary[meshName];
	else								//Return error if key does not exist in library
	{
		std::string noAssetError = "Error: '";
		noAssetError += meshName;
		noAssetError += "' Mesh not found ";
		printf(noAssetError.c_str());
		return nullptr;
	}
}

