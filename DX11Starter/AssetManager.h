#pragma once
#include <d3d11.h>
#include <map>
#include "Mesh.h"
#include "Material.h"
#include "SimpleShader.h"

/* Summary: The Ratchet Asset Manager is responsible for holding all materials, textures, sounds,etc ...
*Notes:
* Make Singleton
*Loads Assets
*/

class AssetManager
{
public:
	static AssetManager& getInstance();  //returns instance of asset manager
	~AssetManager();

	//Create/Import Methods
	void ImportMesh(char* meshName, char* meshFile, //Holds the file path to the model that needs to be loaded
		ID3D11Device* drawDevice);	//Reference to directX 11 Device need to create buffers		

	//Add Methods
	void StoreVShader(char* vShaderName, SimpleVertexShader* vShader);
	void StorePShader(char* pShaderName, SimplePixelShader* pShader);
	void StoreTexture(char* textureName, ID3D11ShaderResourceView* texture);
	void StoreSampler(char* textSamplerName, ID3D11SamplerState* sampler);
	void StoreMaterial(char * matName, Material * materialPtr);
	void StoreMesh(char* meshName, Mesh* meshPtr);

	//Get Methods
	SimpleVertexShader* GetVShader(char* vShaderName);
	SimplePixelShader* GetPShader(char* pShaderName);
	ID3D11ShaderResourceView* GetTexture(char* textureName);
	ID3D11SamplerState* GetSampler(char* samplerName);
	Material* GetMaterial(char* materialName);
	Mesh* GetMesh(char* meshName);

private:
	AssetManager();
	//Stops the compiler from generating methods to copy objects
	AssetManager(AssetManager const& copy);
	AssetManager& operator=(AssetManager const& copy);
	////////////////////////////////////////////////////////////

	std::unordered_map<char*, SimpleVertexShader*> vertexShaderLibrary;
	std::unordered_map<char*, SimplePixelShader*> pixelShaderLibrary;
	std::unordered_map<char*, ID3D11ShaderResourceView*> textureLibrary;
	std::unordered_map<char*, ID3D11SamplerState*> textureSamplerLibrary;
	std::unordered_map<char*, Material*> materialLibrary;
	std::unordered_map<char*, Mesh*> meshLibrary;
	
};

