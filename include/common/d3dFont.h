#ifndef D3DFONT_H
#define D3DFONT_H

#include "d3dDebug.h"

#include <FW1FontWrapper.h>
#pragma  comment(lib, "FW1FontWrapper.lib")

class D3DFont
{
public:
	D3DFont() {};

    void init(ID3D11Device *pD3D11Device, LPCWSTR fontType)
	{
		HRESULT hr = FW1CreateFactory(FW1_VERSION, &pFW1Factory);
		hr = pFW1Factory->CreateFontWrapper(pD3D11Device, fontType, &pFontWrapper);
		DebugHR(hr);
		pFW1Factory->Release();
	}

	void drawText(ID3D11DeviceContext *pD3D11DeivceContext, WCHAR *text, 
		          float fontSize, float posX, float posY, UINT fontCoor);

private:
	IFW1Factory *pFW1Factory;
	IFW1FontWrapper *pFontWrapper;
};

void D3DFont::drawText(ID3D11DeviceContext *pD3D11DeivceContext, WCHAR *text, 
					float fontSize, float posX, float posY, UINT fontColor)
{

	pFontWrapper->DrawString(
		pD3D11DeivceContext,
		text,// String
		fontSize,// D3DFont size
		posX,// X position
		posY,// Y position
		fontColor,// Text color, 0xAaBbGgRr
		FW1_RESTORESTATE// Flags (for example FW1_RESTORESTATE to keep context states unchanged)
		);
}

#endif
