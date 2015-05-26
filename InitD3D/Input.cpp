#include "Input.h"

namespace byhj
{
    Input::Input()
    {
    }
    
    Input::Input(const Input &rhs)
    {
    }
    
    Input::~Input()
    {
    }
    
    void Input::Init()
    {
    	for (int i = 0; i != 256; ++i)
    		keys[i] = false;
    }
    
    void Input::KeyDown(unsigned int input)
    {
    	keys[input] = true;
    }
    
    void Input::KeyUp(unsigned int input)
    {
    	keys[input] = false;
    }
    
    //Return what state the keys if down
    
    bool Input::IsKeyDown(unsigned int key)
    {
    	return keys[key];
    }
}

