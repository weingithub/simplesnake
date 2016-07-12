#ifndef _SIMPLE_SNAKE_H
#define _SIMPLE_SNAKE_H

#include "appdefine.h"

class CSimpleSnake
{
public:
    CSimpleSnake();
    ~CSimpleSnake();

    enum
    {
        DIRECTION_RIGHT = 0X1,
        DIRECTION_LEFT,
        DIRECTION_UP,
        DIRECTION_DOWN,        
    };
    
    int Init(uint32_t x, uint32_t y);

    //int DrawBox(int x, int y, int row, int col, int fcolor,int bcolor, char ch);
    //int DrawBox(int x, int y, int fcolor,int bcolor, char ch);
    int DrawBox(char ch);
    
    int Run();
    
    int Move(); 

private:
    int GetTerminalSize();    
    
private:
    uint8_t  m_direction;
    uint32_t m_left;
    uint32_t m_right;
    uint32_t m_up;
    uint32_t m_down;
};

#endif //_SIMPLE_SNAKE_H

