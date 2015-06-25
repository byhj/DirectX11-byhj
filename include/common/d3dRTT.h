#ifndef D3DRTT_H
#define D3DRTT_H

#include "common/d3dApp.h"
#include <common/d3dShader.h>

class D3DRTT
{
public:
	D3DRTT()
	{
		m_pInputLayout        = NULL;
		m_pMVPBuffer          = NULL;
		m_pVertexBuffer       = NULL;
		m_pIndexBuffer        = NULL;
	}

	void Render(ID3D11DeviceContext *pD3D11DeviceContext, ID3D11ShaderResourceView *pTexture,const XMMATRIX &Model,  
		        const XMMATRIX &View, const XMMATRIX &Proj);

	void shutdown()
	{
		    ReleaseCOM(m_pRenderTargetView  )
			ReleaseCOM(m_pMVPBuffer         )
			ReleaseCOM(m_pVertexBuffer      )
			ReleaseCOM(m_pIndexBuffer       )
	}

	void init_window(int ScreenWidth , int ScreenHeight, int BitmapWidth, int BitmapHeight);
	bool init_buffer (ID3D11Device *pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext);
	bool init_shader (ID3D11Device *pD3D11Device, HWND hWnd);

private:

	struct MatrixBuffer
	{
		XMMATRIX  model;
		XMMATRIX  view;
		XMMATRIX  proj;

	};
	MatrixBuffer cbMatrix;

	struct  Vertex
	{
		D3DXVECTOR3 Pos;
		D3DXVECTOR2 Tex;
	};

	ID3D11RenderTargetView   *m_pRenderTargetView;
	ID3D11Buffer             *m_pMVPBuffer;
	ID3D11Buffer             *m_pVertexBuffer;
	ID3D11Buffer             *m_pIndexBuffer;
	ID3D11SamplerState       *m_pTexSamplerState;
	ID3D11InputLayout        *m_pInputLayout;


	int m_VertexCount;
	int m_IndexCount;

    int m_posX  ;
	int m_posY  ; 
	int m_width ; 
	int m_height;

	Shader D3DRTTShader;
};

void D3DRTT::init_window(int posX, int posY, int width, int height)
{
    m_posX  = posX;
	m_posY  = posY;
	m_width = width; 
	m_height = height;
}

void D3DRTT::Render(ID3D11DeviceContext *pD3D11DeviceContext, ID3D11ShaderResourceView *pTexture, const XMMATRIX &Model,  
				  const XMMATRIX &View, const XMMATRIX &Proj)
{

	cbMatrix.model  = XMMatrixTranspose(Model);
	cbMatrix.view   = XMMatrixTranspose(View);
	cbMatrix.proj   = XMMatrixTranspose(Proj);
	pD3D11DeviceContext->UpdateSubresource(m_pMVPBuffer, 0, NULL, &cbMatrix, 0, 0 );
	pD3D11DeviceContext->VSSetConstantBuffers( 0, 1, &m_pMVPBuffer);

	unsigned int stride;
	unsigned int offset;
	stride = sizeof(Vertex); 
	offset = 0;

	pD3D11DeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	pD3D11DeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	pD3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pD3D11DeviceContext->PSSetShaderResources(0, 1, &pTexture);  
	pD3D11DeviceContext->PSSetSamplers( 0, 1, &m_pTexSamplerState );

	D3DRTTShader.use(pD3D11DeviceContext);
	pD3D11DeviceContext->DrawIndexed(m_IndexCount, 0, 0);

}

bool D3DRTT::init_buffer(ID3D11Device *pD3D11Device, ID3D11DeviceContext *pD3D11DeviceContext)
{
	HRESULT hr;

	Vertex *VertexData;
	unsigned long *IndexData;
	m_VertexCount = 6;
	m_IndexCount = 6;

	VertexData = new Vertex[m_VertexCount];
	if (!VertexData)
	{
		return false;
	}

	IndexData = new unsigned long[m_IndexCount];
	if (!IndexData)
	{
		return false;
	}

	memset(VertexData, 0, sizeof(Vertex) * m_VertexCount);

	for (int i = 0; i != m_IndexCount; ++i)
	{
		IndexData[i] = i;
	}

	// First triangle.
	VertexData[0].Pos = D3DXVECTOR3(m_posX, m_posY, 0.0f);  // Top left.
	VertexData[0].Tex = D3DXVECTOR2(0.0f, 0.0f);

	VertexData[1].Pos = D3DXVECTOR3(m_posX + m_width, m_posY - m_height, 0.0f);  // Bottom right.
	VertexData[1].Tex = D3DXVECTOR2(1.0f, 1.0f);

	VertexData[2].Pos = D3DXVECTOR3(m_posX, m_posY - m_height, 0.0f);  // Bottom left.
	VertexData[2].Tex = D3DXVECTOR2(0.0f, 1.0f);

	// Second triangle.
	VertexData[3].Pos = D3DXVECTOR3(m_posX, m_posY, 0.0f);   // Top left.
	VertexData[3].Tex = D3DXVECTOR2(0.0f, 0.0f);

	VertexData[4].Pos = D3DXVECTOR3(m_posX + m_width, m_posY, 0.0f); // Top right.
	VertexData[4].Tex = D3DXVECTOR2(1.0f, 0.0f);

	VertexData[5].Pos = D3DXVECTOR3(m_posX + m_width, m_posY - m_height, 0.0f);  // Bottom right.
	VertexData[5].Tex = D3DXVECTOR2(1.0f, 1.0f);

	///////////////////////////Index Buffer ////////////////////////////////

	// Set up the description of the static vertex buffer.
	D3D11_BUFFER_DESC VertexBufferDesc;
	VertexBufferDesc.Usage               = D3D11_USAGE_DEFAULT;
	VertexBufferDesc.ByteWidth           = sizeof(Vertex) * m_VertexCount;
	VertexBufferDesc.BindFlags           = D3D11_BIND_VERTEX_BUFFER;
	VertexBufferDesc.CPUAccessFlags      = 0;
	VertexBufferDesc.MiscFlags           = 0;
	VertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	D3D11_SUBRESOURCE_DATA VBO;
	VBO.pSysMem          = VertexData;
	VBO.SysMemPitch      = 0;
	VBO.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	hr = pD3D11Device->CreateBuffer(&VertexBufferDesc, &VBO, &m_pVertexBuffer);
	DebugHR(hr);

	/////////////////////////////////Index Buffer ///////////////////////////////////////

	// Set up the description of the static index buffer.
	D3D11_BUFFER_DESC IndexBufferDesc;
	IndexBufferDesc.Usage               = D3D11_USAGE_DEFAULT;
	IndexBufferDesc.ByteWidth           = sizeof(unsigned long) * m_IndexCount;
	IndexBufferDesc.BindFlags           = D3D11_BIND_INDEX_BUFFER;
	IndexBufferDesc.CPUAccessFlags      = 0;
	IndexBufferDesc.MiscFlags           = 0;
	IndexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	D3D11_SUBRESOURCE_DATA IBO;
	IBO.pSysMem          = IndexData;
	IBO.SysMemPitch      = 0;
	IBO.SysMemSlicePitch = 0;

	hr = pD3D11Device->CreateBuffer(&IndexBufferDesc, &IBO, &m_pIndexBuffer);
	DebugHR(hr);

	////////////////////////////////MVP Buffer//////////////////////////////////////

	D3D11_BUFFER_DESC mvpBufferDesc;	
	ZeroMemory(&mvpBufferDesc, sizeof(D3D11_BUFFER_DESC));
	mvpBufferDesc.Usage          = D3D11_USAGE_DEFAULT;
	mvpBufferDesc.ByteWidth      = sizeof(MatrixBuffer);
	mvpBufferDesc.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
	mvpBufferDesc.CPUAccessFlags = 0;
	mvpBufferDesc.MiscFlags      = 0;
	hr = pD3D11Device->CreateBuffer(&mvpBufferDesc, NULL, &m_pMVPBuffer);
	DebugHR(hr);

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

	// Create the texture sampler state.
	hr = pD3D11Device->CreateSamplerState(&samplerDesc, &m_pTexSamplerState);
	DebugHR(hr);

	return true;
}


bool D3DRTT::init_shader(ID3D11Device *pD3D11Device, HWND hWnd)
{
	HRESULT result;

	D3D11_INPUT_ELEMENT_DESC pInputLayoutDesc[2];
	pInputLayoutDesc[0].SemanticName = "POSITION";
	pInputLayoutDesc[0].SemanticIndex = 0;
	pInputLayoutDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	pInputLayoutDesc[0].InputSlot = 0;
	pInputLayoutDesc[0].AlignedByteOffset = 0;
	pInputLayoutDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	pInputLayoutDesc[0].InstanceDataStepRate = 0;

	pInputLayoutDesc[1].SemanticName = "TEXCOORD";
	pInputLayoutDesc[1].SemanticIndex = 0;
	pInputLayoutDesc[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	pInputLayoutDesc[1].InputSlot = 0;
	pInputLayoutDesc[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	pInputLayoutDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	pInputLayoutDesc[1].InstanceDataStepRate = 0;

	unsigned numElements = ARRAYSIZE(pInputLayoutDesc);

	D3DRTTShader.init(pD3D11Device, hWnd);
	D3DRTTShader.attachVS(L"rtt.vsh", pInputLayoutDesc, numElements);
	D3DRTTShader.attachPS(L"rtt.psh");
	D3DRTTShader.end();

	return true;
}


#endif