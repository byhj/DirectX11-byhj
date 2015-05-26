#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <windows.h>

namespace byhj
{
    //Global
    const bool FULL_SCREEN   = false;
    const bool VSYNC_ENABLED = true;
    const float SCREEN_DEPTH = 1000.0f;
    const float SCREEN_NEAR  = 0.1f;
    
    class Graphics
    {
    public:
    	Graphics();
    	Graphics(const Graphics &);
    	~Graphics();
    
    	bool Init(int, int, HWND);
    	void Shutdown();
    	bool Render();
    
    private:
    	bool RenderGraphics();
    
    };
    
}
#endif //Graphics