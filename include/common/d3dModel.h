#ifndef D3DMODEL_H
#define D3DMODEL_H

// Std. Includes
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "d3dMesh.h"

int TextureFromFile(const char* path, std::string directory);

class D3DModel
{
public:
	D3DModel() {}
	
	void loadModel(std::string path);

   // Processes a node in a recursive fashion. Processes each individual mesh located at 
	//the node and repeats this process on its children nodes (if any).
	void processNode(aiNode* node, const aiScene* scene);

	//Processes a mesh and return the data information
	D3DMesh processMesh(aiMesh* mesh, const aiScene* scene);

	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);


	const std::vector<XMFLOAT3> & GetPos()
	{
		return vPos;
	}
	const std::vector<unsigned long> & GetIndex()
	{
		return vIndex;
	}
private:

	//One model may include many meshes
	std::vector<D3DMesh> meshes;
	std::string directory;
	std::vector<Texture> textures_loaded;	

	std::vector<int> meshIndexStart;
	std::vector<int> meshTexture;

	std::vector<XMFLOAT3> vPos;
	std::vector<unsigned long> vIndex;	
};


void D3DModel::loadModel(std::string path)
{
	// Read file via ASSIMP
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
	
	// Check for errors
	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
	{
		std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
		return;
	}

	// Retrieve the directory path of the filepath
	this->directory = path.substr(0, path.find_last_of('/'));

	// Process ASSIMP's root node recursively
	this->processNode(scene->mRootNode, scene);
}

std::vector<Texture>  D3DModel::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
	std::vector<Texture> textures;
	for (int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		// Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		bool skip = false;
		for (int j = 0; j < textures_loaded.size(); j++)
		{
			if (textures_loaded[j].path == str)
			{
				textures.push_back(textures_loaded[j]);
				skip = true; // A texture with the same filepath has already been loaded, continue to next one. (optimization)
				break;
			}
		}
		if (!skip)
		{   // If texture hasn't been loaded already, load it
			Texture texture;
			texture.type = typeName;
			texture.path = str;
			textures.push_back(texture);
			this->textures_loaded.push_back(texture);  // Store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
		}
	}
	return textures;
}

void setColor(aiColor3D &c, XMFLOAT4 &m)
{
	m.x = c[0];
	m.y = c[1];
	m.z = c[2];
}

void setBlend(float blend, Material &mat)
{
	mat.ambient.w = blend;
	mat.specular.w = blend;
	mat.diffuse.w = blend;
	mat.emissive.w = blend;
}

D3DMesh D3DModel::processMesh(aiMesh* mesh, const aiScene* scene)
{
	//Mesh Data to fill
	std::vector<Vertex> vertices;
	std::vector<unsigned long> indices;
	std::vector<Texture> textures;
	std::vector<Material> MaterialData;

	XMVECTOR v[3];
	XMVECTOR uv[3];

	// Walk through each of the mesh's vertices
	for (int i = 0, j = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		XMFLOAT3 pos; // We declare a placeholder std::vector since assimp uses its own std::vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder XMFloat3 first.
		XMFLOAT3 normal;

		// Positions
		pos.x = mesh->mVertices[i].x;
		pos.y = mesh->mVertices[i].y;
		pos.z = mesh->mVertices[i].z;
		vertex.Position = pos;
		vPos.push_back(pos);

		// Normals
		normal.x = mesh->mNormals[i].x;
		normal.y = mesh->mNormals[i].y;
		normal.z = mesh->mNormals[i].z;
		vertex.Normal = normal;

		// Texture Coordinates
		if (mesh->mTextureCoords[0]) // Does the mesh contain texture coordinates?
		{
			XMFLOAT2 vec;
			// A vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
			// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
		}
		else
			vertex.TexCoords = XMFLOAT2(0.0f, 0.0f);

		//Process one Vertex
		vertices.push_back(vertex);

		//////////////////Calc the Tangent and BiTangent///////////////////////////////
		v[j]  = XMLoadFloat3(&vertex.Position);
		uv[j] = XMLoadFloat2(&vertex.TexCoords );
		if ( (j+1) % 3 == 0)
		{
			// Edges of the triangle : postion delta
			XMVECTOR deltaPos1 = v[1]  - v[0];
			XMVECTOR deltaPos2 = v[2]  - v[0];
			XMVECTOR deltaUV1  = uv[1] - uv[0];
			XMVECTOR deltaUV2  = uv[2] - uv[0];

			float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
			XMVECTOR tangent = (deltaPos1 * deltaUV2.y   - deltaPos2 * deltaUV1.y) * r;
			XMVECTOR bitangent = (deltaPos2 * deltaUV1.x   - deltaPos1 * deltaUV2.x) * r;

			XMStoreFloat3(&vertices[i].Tangent, tangent);
			XMStoreFloat3(&vertices[i-1].Tangent, tangent);
			XMStoreFloat3(&vertices[i-2].Tangent, tangent);

			XMStoreFloat3(&vertices[i].BiTangent, bitangent);
			XMStoreFloat3(&vertices[i-1].BiTangent, bitangent);
			XMStoreFloat3(&vertices[i-2].BiTangent, bitangent);
			j = 0;
		}
		else
			++j;
	}

	// Now walk through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
	for (int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		// Retrieve all indices of the face and store them in the indices std::vector
		for (int j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
			//
			vIndex.push_back(face.mIndices[j]);
		}
	}
	Material mat;

	// Process materials
	if (mesh->mMaterialIndex >= 0)
	{
		int tt = mesh->mMaterialIndex;
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		// We assume a convention for sampler names in the shaders. Each diffuse texture should be named
		// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
		// Same applies to other texture as the following list summarizes:
		// Diffuse: texture_diffuseN
		// Specular: texture_specularN
		// Normal: texture_normalN

		aiColor3D ambient;
	    if(AI_SUCCESS == material->Get(AI_MATKEY_COLOR_AMBIENT, ambient) )
			setColor(ambient, mat.ambient);
		//std::cout << mat.ambient.w << std::endl;
	    aiColor3D diffuse;
		if(AI_SUCCESS == material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse) )
			setColor(diffuse, mat.diffuse);

		aiColor3D  speucular;
		if(AI_SUCCESS == material->Get(AI_MATKEY_COLOR_SPECULAR, speucular) )
			setColor(speucular, mat.specular);

		aiColor3D  emissive;
		if(AI_SUCCESS == material->Get(AI_MATKEY_COLOR_EMISSIVE, emissive) )
			setColor(emissive, mat.emissive);

		//float shininess = 0.0;
		//if(AI_SUCCESS == material->Get(AI_MATKEY_SHININESS, shininess))
		//	mat.shininess = shininess;
		
		float blend;
		material->Get(AI_MATKEY_OPACITY , blend);
		if (blend < 1.0f)
			setBlend(blend, mat);
		//std::cout << mat.ambient.w << std::endl;

		// 1. Diffuse maps
		std::vector<Texture> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		// 2. Specular maps
		std::vector<Texture> specularMaps = this->loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());


		// 3.normal maps
		std::vector<Texture> normalMaps = this->loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
	}

	// Return a mesh object created from the extracted mesh data
	return D3DMesh(vertices, indices, textures, mat);
}

void D3DModel::processNode(aiNode* node, const aiScene* scene)
{
	// Process each mesh located at the current node
	for (int i = 0; i < node->mNumMeshes; i++)
	{
		// The node object only contains indices to index the actual objects in the scene. 
		// The scene contains all the data, node is just to keep stuff organized (like relations between nodes).
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		this->meshes.push_back(this->processMesh(mesh, scene));
	}

	// After we've processed all of the meshes (if any) we then recursively process each of the children nodes
	for (int i = 0; i < node->mNumChildren; i++)
	{
		this->processNode(node->mChildren[i], scene);
	}

}
#endif