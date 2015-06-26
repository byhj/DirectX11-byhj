#ifndef TEST_H
#define TEST_H

#include "common/d3dModel.h"
#include "common/d3dShader.h"
#include "common/d3dDebug.h"
#include "D3DX10math.h"
class Test
{
public:
	Test()
	{
		m_VertexCount = 0;
		m_IndexCount  = 0;
	}

	void Render(ID3D11DeviceContext *pD3D11DeviceContext, XMMATRIX model, XMMATRIX view, XMMATRIX proj)
	{
		ModelShader.use(pD3D11DeviceContext);

		pD3D11DeviceContext->PSSetSamplers( 0, 1, &m_pTexSamplerState );
		pD3D11DeviceContext->PSSetShaderResources( 0, 2, &m_pTextures[0]);
		cbMatrix.Model = XMMatrixTranspose(model);
		cbMatrix.View  = XMMatrixTranspose(view);
		cbMatrix.Porj  = XMMatrixTranspose(proj);
		pD3D11DeviceContext->UpdateSubresource(m_pMVPBuffer, 0, NULL, &cbMatrix, 0, 0 );
		pD3D11DeviceContext->VSSetConstantBuffers( 0, 1, &m_pMVPBuffer);

		unsigned int stride;
		unsigned int offset;
		stride = sizeof(Vertex); 
		offset = 0;
		pD3D11DeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
		pD3D11DeviceContext->IASetVertexBuffers( 0, 1, &m_pVertexBuffer, &stride, &offset);

		int startIndex = 0;
		for (int i = 0; i < TestModel.meshes.size(); i++)
		{
			float blendFactor[] = {0.4f, 0.4f, 0.4f, 0.3f};
			if (TestModel.meshes[i].mat.ambient.w < 1.0f)
				pD3D11DeviceContext->OMSetBlendState(Transparency, blendFactor, 0xffffffff);

//			pD3D11DeviceContext->RSSetState(m_pRasterState);
			cbMat.ambient = TestModel.meshes[i].mat.ambient;
			cbMat.diffuse = TestModel.meshes[i].mat.diffuse;
			cbMat.specular = TestModel.meshes[i].mat.specular;
			cbMat.emissive = TestModel.meshes[i].mat.emissive;
            for (int j = 0; j != 4; ++j)
				cbMat.hasTex[j] = TestModel.meshes[i].mat.hasTex[j];

			pD3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			pD3D11DeviceContext->UpdateSubresource(m_pMatBuffer, 0, NULL, &cbMat, 0, 0 );
			pD3D11DeviceContext->PSSetConstantBuffers(0, 1, &m_pMatBuffer);

			if (i == 0)
				pD3D11DeviceContext->DrawIndexed(TestModel.meshes[i].m_IndexCount, 0, 0);
			else
				pD3D11DeviceContext->DrawIndexed(TestModel.meshes[i].m_IndexCount, startIndex + TestModel.meshes[i-1].m_IndexCount, 0);

			pD3D11DeviceContext->OMSetBlendState(0, 0, 0xffffffff);
		}
	}


	void init_buffer(ID3D11Device *pD3D11Device);
	void initModel(ID3D11Device *pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext, HWND hWnd);

private:
	D3DModel TestModel;
	void init_shader(ID3D11Device *pD3D11Device, HWND hWnd);
	struct MatrixBuffer
	{
		XMMATRIX Model;
		XMMATRIX View;
		XMMATRIX Porj;
	};
	MatrixBuffer cbMatrix;

	struct MaterialBuffer
	{
		D3DXVECTOR4 ambient ;
		D3DXVECTOR4 diffuse;
		D3DXVECTOR4 specular;
		D3DXVECTOR4 emissive;
		float hasTex[4];
	};
	MaterialBuffer cbMat;

	ID3D11Buffer *m_pIndexBuffer;
	ID3D11Buffer *m_pVertexBuffer;
	ID3D11Buffer *m_pMVPBuffer;
	ID3D11Buffer *m_pMatBuffer;

	Shader ModelShader;

	ID3D11ShaderResourceView *m_pTextures[4];
	ID3D11ShaderResourceView *m_pShaderResourceView;
	ID3D11DepthStencilView   *m_pDepthStencilView;
	ID3D11Texture2D          *m_pDepthStencilBuffer;
	ID3D11RasterizerState    *m_pRasterState;
	ID3D11SamplerState       *m_pTexSamplerState;
	ID3D11BlendState         *Transparency;

	int m_VertexCount;
	int m_IndexCount;

};

void Test::init_buffer(ID3D11Device *pD3D11Device)
{
	HRESULT hr;

	///////////////////////////Vertex Buffer ////////////////////////////////
	for (int i = 0; i != TestModel.meshes.size(); ++i)
		m_VertexCount += TestModel.meshes[i].m_VertexCount;

	// Set up the description of the static vertex buffer.
	D3D11_BUFFER_DESC VertexBufferDesc;
	VertexBufferDesc.Usage               = D3D11_USAGE_DEFAULT;
	VertexBufferDesc.ByteWidth           = sizeof(Vertex) * m_VertexCount;
	VertexBufferDesc.BindFlags           = D3D11_BIND_VERTEX_BUFFER;
	VertexBufferDesc.CPUAccessFlags      = 0;
	VertexBufferDesc.MiscFlags           = 0;
	VertexBufferDesc.StructureByteStride = 0;

	std::vector<Vertex> vpos = TestModel.GetPos();
	// Give the subresource structure a pointer to the vertex data.
	D3D11_SUBRESOURCE_DATA VBO;
	VBO.pSysMem          = &vpos[0];
	VBO.SysMemPitch      = 0;
	VBO.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	hr = pD3D11Device->CreateBuffer(&VertexBufferDesc, &VBO, &m_pVertexBuffer);
	DebugHR(hr);

	/////////////////////////////////Index Buffer ///////////////////////////////////////

	for (int i = 0; i != TestModel.meshes.size(); ++i)
		m_IndexCount += TestModel.meshes[i].m_IndexCount;

	// Set up the description of the static index buffer.
	D3D11_BUFFER_DESC IndexBufferDesc;
	IndexBufferDesc.Usage               = D3D11_USAGE_DEFAULT;
	IndexBufferDesc.ByteWidth           = sizeof(unsigned long) * m_IndexCount;
	IndexBufferDesc.BindFlags           = D3D11_BIND_INDEX_BUFFER;
	IndexBufferDesc.CPUAccessFlags      = 0;
	IndexBufferDesc.MiscFlags           = 0;
	IndexBufferDesc.StructureByteStride = 0;

	std::vector<unsigned long> vIndex = TestModel.GetIndex();
	// Give the subresource structure a pointer to the index data.
	D3D11_SUBRESOURCE_DATA IBO;
	IBO.pSysMem          = &vIndex[0];
	IBO.SysMemPitch      = 0;
	IBO.SysMemSlicePitch = 0;

	hr = pD3D11Device->CreateBuffer(&IndexBufferDesc, &IBO, &m_pIndexBuffer);
	DebugHR(hr);

	///////////////////////////////////////////////////////////////////
	D3D11_BUFFER_DESC mvpDesc;	
	ZeroMemory(&mvpDesc, sizeof(D3D11_BUFFER_DESC));
	mvpDesc.Usage          = D3D11_USAGE_DEFAULT;
	mvpDesc.ByteWidth      = sizeof(MatrixBuffer);
	mvpDesc.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
	mvpDesc.CPUAccessFlags = 0;
	mvpDesc.MiscFlags      = 0;
	hr = pD3D11Device->CreateBuffer(&mvpDesc, NULL, &m_pMVPBuffer);
	DebugHR(hr);

	D3D11_BUFFER_DESC matDesc;	
	ZeroMemory(&matDesc, sizeof(D3D11_BUFFER_DESC));
	matDesc.Usage          = D3D11_USAGE_DEFAULT;
	matDesc.ByteWidth      = sizeof(MaterialBuffer);
	matDesc.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
	matDesc.CPUAccessFlags = 0;
	matDesc.MiscFlags      = 0;
	hr = pD3D11Device->CreateBuffer(&matDesc, NULL, &m_pMatBuffer);
	DebugHR(hr);
	for (int i = 0; i != TestModel.texturePathes.size(); ++i)
	{
	  std::string str = TestModel.texturePathes[i];
	  std::wstring widestr = std::wstring(str.begin(), str.end());
	  const WCHAR *texFile = widestr.c_str();
	  hr = D3DX11CreateShaderResourceViewFromFile(pD3D11Device, texFile, NULL,NULL, &m_pTextures[i], NULL);
	  DebugHR(hr);
	}
}

void Test::initModel(ID3D11Device *pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext, HWND hWnd)
{
	TestModel.loadModel("../../media/objects/ground.obj");

	init_shader(pD3D11Device, hWnd);

	// Create a texture sampler state description.
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	HRESULT hr;
	// Create the texture sampler state.
	hr = pD3D11Device->CreateSamplerState(&samplerDesc, &m_pTexSamplerState);
	DebugHR(hr);

	///////////////////////////////////////////////////////////////////
	D3D11_BLEND_DESC blendDesc;
	ZeroMemory( &blendDesc, sizeof(blendDesc) );
	D3D11_RENDER_TARGET_BLEND_DESC rtbd;
	ZeroMemory( &rtbd, sizeof(rtbd) );
	rtbd.BlendEnable			 = true;
	rtbd.SrcBlend				 = D3D11_BLEND_SRC_COLOR;
	rtbd.DestBlend				 = D3D11_BLEND_BLEND_FACTOR;
	rtbd.BlendOp				 = D3D11_BLEND_OP_ADD;
	rtbd.SrcBlendAlpha			 = D3D11_BLEND_ONE;
	rtbd.DestBlendAlpha			 = D3D11_BLEND_ZERO;
	rtbd.BlendOpAlpha			 = D3D11_BLEND_OP_ADD;
	rtbd.RenderTargetWriteMask	 = D3D10_COLOR_WRITE_ENABLE_ALL;
	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.RenderTarget[0] = rtbd;
	pD3D11Device->CreateBlendState(&blendDesc, &Transparency);
}

void Test::init_shader(ID3D11Device *pD3D11Device, HWND hWnd)
{
	HRESULT hr;
	D3D11_INPUT_ELEMENT_DESC pInputLayoutDesc[5];

	pInputLayoutDesc[0].SemanticName         = "POSITION";
	pInputLayoutDesc[0].SemanticIndex        = 0;
	pInputLayoutDesc[0].Format               = DXGI_FORMAT_R32G32B32_FLOAT;
	pInputLayoutDesc[0].InputSlot            = 0;
	pInputLayoutDesc[0].AlignedByteOffset    = 0;
	pInputLayoutDesc[0].InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
	pInputLayoutDesc[0].InstanceDataStepRate = 0;

	pInputLayoutDesc[1].SemanticName         = "NORMAL";
	pInputLayoutDesc[1].SemanticIndex        = 0;
	pInputLayoutDesc[1].Format               = DXGI_FORMAT_R32G32B32_FLOAT;
	pInputLayoutDesc[1].InputSlot            = 0;
	pInputLayoutDesc[1].AlignedByteOffset    = 12;
	pInputLayoutDesc[1].InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
	pInputLayoutDesc[1].InstanceDataStepRate = 0;

	pInputLayoutDesc[2].SemanticName         = "TEXCOORD";
	pInputLayoutDesc[2].SemanticIndex        = 0;
	pInputLayoutDesc[2].Format               = DXGI_FORMAT_R32G32_FLOAT;
	pInputLayoutDesc[2].InputSlot            = 0;
	pInputLayoutDesc[2].AlignedByteOffset    = 24;
	pInputLayoutDesc[2].InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
	pInputLayoutDesc[2].InstanceDataStepRate = 0;

	pInputLayoutDesc[3].SemanticName         = "TANGENT";
	pInputLayoutDesc[3].SemanticIndex        = 0;
	pInputLayoutDesc[3].Format               = DXGI_FORMAT_R32G32B32_FLOAT;
	pInputLayoutDesc[3].InputSlot            = 0;
	pInputLayoutDesc[3].AlignedByteOffset    = 32;
	pInputLayoutDesc[3].InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
	pInputLayoutDesc[3].InstanceDataStepRate = 0;

	pInputLayoutDesc[4].SemanticName         = "BITANGENT";
	pInputLayoutDesc[4].SemanticIndex        = 0;
	pInputLayoutDesc[4].Format               = DXGI_FORMAT_R32G32B32_FLOAT;
	pInputLayoutDesc[4].InputSlot            = 0;
	pInputLayoutDesc[4].AlignedByteOffset    = 44;
	pInputLayoutDesc[4].InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
	pInputLayoutDesc[4].InstanceDataStepRate = 0;

	unsigned numElements = ARRAYSIZE(pInputLayoutDesc);

	ModelShader.init(pD3D11Device, hWnd);
	ModelShader.attachVS(L"model.vsh", pInputLayoutDesc, numElements);
	ModelShader.attachPS(L"model.psh");
	ModelShader.end();
}

#endif