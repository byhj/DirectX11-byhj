#ifndef D3DMESH_H
#define D3DMESH_H

#ifdef _WIN32
#define _XM_NO_INTRINSICS_
#endif 

#include <d3d11.h>
#include <D3DX11.h>
#include <xnamath.h>

#include "common/d3dShader.h"
#include "common/d3dDebug.h"

// Std. Includes
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

//Assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

struct Vertex 
{
	XMFLOAT3 Position;
	XMFLOAT3 Normal;
	XMFLOAT2 TexCoords;

	//Some model not need
	XMFLOAT3 Tangent;
	XMFLOAT3 BiTangent;
};



struct Material
{
	Material()
	{
	   ambient   = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	   diffuse   = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	   specular  = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	   emissive  = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	  // shininess = 32.0f;
	}
	Material (const Material &mat)
	{
		    this->ambient   = mat.ambient  ;
			this->diffuse   = mat.diffuse  ;
			this->specular  = mat.specular ;
			this->emissive  = mat.emissive ;
			//this->shininess = mat.shininess;
	}
	Material &operator = (const Material &mat)
	{
		this->ambient   = mat.ambient  ;
		this->diffuse   = mat.diffuse  ;
		this->specular  = mat.specular ;
		this->emissive  = mat.emissive ;
		//this->shininess = mat.shininess;

		return *this;
	}

	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
	XMFLOAT4 specular;
	XMFLOAT4 emissive;
	//float shininess;
};



struct Texture 
{
	std::string type;
	aiString path;
};

class D3DMesh 
{

public:
	D3DMesh() {}

	D3DMesh(std::vector<Vertex> vertices, std::vector<unsigned long> indices,
		    std::vector<Texture> textures, Material mat)
	{
		this->VertexData = vertices;
		this->IndexData = indices;
		this->textures = textures;
		this->mat = mat;
	}

public:

	void load_texture(ID3D11Device *pD3D11Device, WCHAR *texFile);

	int m_VertexCount;
	int m_IndexCount;

	/*  Mesh Data  */
	std::vector<Vertex> VertexData;
	std::vector<unsigned long> IndexData;
	

	Material mat;
};



#endif