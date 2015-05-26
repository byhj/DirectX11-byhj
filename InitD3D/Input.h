#ifndef INPUTCLASS_H
#define INPUTCLASS_H

namespace byhj
{

    class Input
    {
    public:
    	Input();
    	Input(const Input &);
    	~Input();
    
    	void Init();
    
    	void KeyDown(unsigned int);
    	void KeyUp(unsigned int);
    	bool IsKeyDown(unsigned int);
    
    private:
    	bool keys[256];
    };  //Class
    
} //Namespace 

#endif //InputClass