#ifndef D3DMODEL_H
#define D3DMODEL_H

#ifdef _WIN32
#define _XM_NO_INTRINSICS_
#endif 

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

#include <d3d11.h>
#include <D3DX11.h>
#include <xnamath.h>
#include <D3DX10math.h>

struct Vertex {
	// Position
	XMFLOAT3 Position;
	// Normal
	XMFLOAT3 Normal;
	// TexCoords
	XMFLOAT2 TexCoords;

	XMFLOAT3 Tangent;

	XMFLOAT3 BiTangent;
};


struct Material
{
	D3DXVECTOR4 ambient;
	D3DXVECTOR4 diffuse;
	D3DXVECTOR4 specular;
	D3DXVECTOR4 emissive;
	float hasTex[4];
};

struct D3DMesh 
{
	std::vector<Vertex> VertexData;
	std::vector<unsigned long> IndexData;
	std::vector<int> textureIndex;
	Material mat;

	int m_VertexCount;
	int m_IndexCount;
};


class D3DModel
{
public:
	D3DModel(){}

	
	//Loading the model form file if support this format
	void loadModel(std::string path);
	void processNode(aiNode* node, const aiScene* scene);
	void processMesh(aiMesh* mesh, const aiScene* scene);

	int loadMatTex(int &index, aiMaterial* mat, aiTextureType type, std::string typeName);

	std::vector<Vertex>  GetPos()
	{
		return vVertex;
	}
	std::vector<unsigned long> GetIndex()
	{
		return vIndex;
	}

public:
	//One model may include many meshes
	std::vector<D3DMesh> meshes;
	std::vector<std::string> texturePathes;
	std::string directory;

	std::vector<Vertex> vVertex;
	std::vector<XMFLOAT3> vPos;
	std::vector<unsigned long> vIndex;	
};


void D3DModel::loadModel(std::string path)
{
	// Read file via ASSIMP
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
	
	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) 
	{
		std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
		return;
	}
	// Retrieve the directory path of the filepath
	directory = path.substr(0, path.find_last_of('/'));

	// Process ASSIMP's root node recursively
	this->processNode(scene->mRootNode, scene);
}

int  D3DModel::loadMatTex(int &index, aiMaterial* mat, aiTextureType type, std::string typeName)
{
	if (!mat->GetTextureCount(type))
		return 0;

	for (int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		std::string path = directory + '/' +str.C_Str();
		// Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		bool skip = false;
		for (int j = 0; j < texturePathes.size(); j++)
		{
			if (texturePathes[j] == path)
			{
				index = j;
				skip = true; // A texture with the same filepath has already been loaded, continue to next one. (optimization)
				break;
			}
		}
		if (!skip)
		{   // If texture hasn't been loaded already, load it
			texturePathes.push_back(path);
			index = texturePathes.size();
		}
	}
	return 1;
}

void setColor(aiColor3D &c, D3DXVECTOR4 &m)
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

void D3DModel::processMesh(aiMesh* mesh, const aiScene* scene)
{

	D3DMesh d3dMesh;

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
		d3dMesh.VertexData.push_back(vertex);

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

			XMStoreFloat3(&d3dMesh.VertexData[i].Tangent,  tangent);
			XMStoreFloat3(&d3dMesh.VertexData[i-1].Tangent, tangent);
			XMStoreFloat3(&d3dMesh.VertexData[i-2].Tangent, tangent);

			XMStoreFloat3(&d3dMesh.VertexData[i].BiTangent,  bitangent);
			XMStoreFloat3(&d3dMesh.VertexData[i-1].BiTangent, bitangent);
			XMStoreFloat3(&d3dMesh.VertexData[i-2].BiTangent, bitangent);
			j = 0;
		}
		else
			++j;
	}
	 for (int i = 0; i != d3dMesh.VertexData.size(); ++i)
		 vVertex.push_back(d3dMesh.VertexData[i]);

	// Now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
	for (int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		// Retrieve all indices of the face and store them in the indices std::vector
		for (int j = 0; j < face.mNumIndices; j++)
		{
			d3dMesh.IndexData.push_back(face.mIndices[j]);
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
			//mat.shininess = shininess;
		
		float blend;
		material->Get(AI_MATKEY_OPACITY , blend);
			setBlend(blend, mat);
		//std::cout << mat.ambient.w << std::endl;

		int texIndex = 0;
		// 1. Diffuse maps
		mat.hasTex[0] = loadMatTex(texIndex, material, aiTextureType_AMBIENT, "texture_ambient");
		if (mat.hasTex[0])
		d3dMesh.textureIndex.push_back(texIndex);

		// 1. Diffuse maps
	    mat.hasTex[1] = loadMatTex(texIndex, material, aiTextureType_DIFFUSE, "texture_diffuse");
		if (mat.hasTex[1])
			d3dMesh.textureIndex.push_back(texIndex);;

		// 2. Specular maps
		mat.hasTex[2] = loadMatTex(texIndex, material, aiTextureType_SPECULAR, "texture_specular");
		if (mat.hasTex[2])
			d3dMesh.textureIndex.push_back(texIndex);

		// 3.normal maps
		mat.hasTex[3] = loadMatTex(texIndex, material, aiTextureType_HEIGHT, "texture_normal");
		if (mat.hasTex[3])
			d3dMesh.textureIndex.push_back(texIndex);
	}
	d3dMesh.mat  = mat;
	d3dMesh.m_IndexCount = d3dMesh.IndexData.size();
	d3dMesh.m_VertexCount = d3dMesh.VertexData.size();

	meshes.push_back(d3dMesh);
}

void D3DModel::processNode(aiNode* node, const aiScene* scene)
{
	// Process each mesh located at the current node
	for (int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		processMesh(mesh, scene);
	}
	for (int i = 0; i < node->mNumChildren; i++)
	{
		this->processNode(node->mChildren[i], scene);
	}

}


#endif