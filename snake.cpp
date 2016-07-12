#include "snake.h"
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include<sys/ioctl.h>

using namespace std;

// 清除屏幕 
#define CLEAR() printf("\033[2J") 

// 上移光标 
#define MOVEUP(x) printf("\033[%dA", (x)) 

// 下移光标 
#define MOVEDOWN(x) printf("\033[%dB", (x)) 

// 左移光标 
#define MOVELEFT(y) printf("\033[%dD", (y)) 

// 右移光标 
#define MOVERIGHT(y) printf("\033[%dC",(y)) 

// 定位光标 
//先y坐标，再x坐标
#define MOVETO(x,y) printf("\033[%d;%dH", (y), (x)) 

// 光标复位 
#define RESET_CURSOR() printf("\033[H") 

// 隐藏光标 
#define HIDE_CURSOR() printf("\033[?25l") 

// 显示光标 
#define SHOW_CURSOR() printf("\033[?25h") 

CSimpleSnake::CSimpleSnake():m_direction(0),m_left(0),m_right(0),m_up(0),m_down(0)
{
    
}

CSimpleSnake::~CSimpleSnake()
{
    
}

int CSimpleSnake::Init(uint32_t x, uint32_t y)
{
    //draw a x*y rectangle
    CLEAR();
        
    m_right = x;
    m_down = y;
   
   
    GetTerminalSize();

    DrawBox('*');
    return 0;
}

int CSimpleSnake::DrawBox(char ch)
{
    //画一个游戏盒子

    uint8_t direction = DIRECTION_RIGHT;

    CLEAR();
    
    while(true)
    {
        if (DIRECTION_RIGHT == direction)
        {
            MOVETO(0, 0);
            
            for(uint32_t i = 0; i < m_right; ++i)
            {
                //printf("\033[44;34m#\033[0m");
                cout<<ch<<flush;
                usleep(10000);
            }
            
            direction = DIRECTION_DOWN;
        }
        else if (DIRECTION_DOWN == direction)
        {
            for(uint32_t j = 1; j < m_down; ++j)
            {
                MOVETO(m_right, j);
                //printf("\033[44;34m#\033[0m");
                cout<<ch<<flush;
                usleep(10000);
            }
            
            direction =  DIRECTION_LEFT;
        }
        else if(DIRECTION_LEFT == direction)
        {
            MOVETO(0, m_down);
            
            for(uint32_t i = 0; i < m_right; ++i)
            {
                //printf("\033[44;34m#\033[0m");
                cout<<ch<<flush;
                usleep(10000);
            }
            
            cout<<flush;
            
            direction = DIRECTION_UP;
        }
        else if (DIRECTION_UP == direction)
        {
            for(uint32_t j = 1; j < m_down; ++j)
            {
                MOVETO(0, j);
                //printf("\033[44;34m#\033[0m");
                cout<<ch<<flush;
                usleep(10000);
            }
            
            break;
        }
    }
    
    printf("\033[0m");  //关闭所有属性 
    MOVETO(0, m_down+2);
    //MOVETO(2, 0);
}

int CSimpleSnake::GetTerminalSize()
{
    struct winsize size;
    
    ioctl(STDIN_FILENO,TIOCGWINSZ,&size);
    printf("%d\n",size.ws_col);
    printf("%d\n",size.ws_row);
    
    //当横轴大于终端的列值时，取终端列值
    if (m_right > size.ws_col)
    {
        m_right = size.ws_col;
    }
    
    //当纵轴大于等于终端的行值时，取终端行值-1
    if (m_down > size.ws_row)
    {
        m_down = size.ws_row - 1;
    }
    
    return 0;
}

int CSimpleSnake::Run()
{
    
}

int CSimpleSnake::Move()
{
    
}

int StringToUInt(const char * ptr, uint32_t & result)
{
    //use byte convert;
    result = 0;
    
    int len = strlen(ptr);
    
    for (int i = 0; i < len; ++i)
    {
        result *= 10;
        result += ptr[i]-'0'; 
    }
    
    return 0;
}

int main(int argc, char **argv)
{
    uint32_t x =0, y = 0;
    
    if (3 != argc)
    {
        x = 30;
        y = 30;
    }
    else
    {
        StringToUInt(argv[1], x);
        StringToUInt(argv[2], y);
    }
    
    CSimpleSnake simpledemo;
    
    simpledemo.Init(x, y);
    
    //simpledemo.Run();
}
